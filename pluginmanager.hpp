#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <chrono>
#include <format>
#include <variant>

#include "scraper_api.h" 
#include "plugin_api.h"

#ifdef _WIN32
    #include <windows.h>
    #define LOAD_LIB(path)       LoadLibraryA(path)
    #define GET_FUNC(lib, name)  GetProcAddress((HMODULE)lib, name)
    #define FREE_LIB(lib)        FreeLibrary((HMODULE)lib)
#else
    #include <dlfcn.h>
    #define LOAD_LIB(path)       dlopen(path, RTLD_LAZY)
    #define GET_FUNC(lib, name)  dlsym(lib, name)
    #define FREE_LIB(lib)        dlclose(lib)
#endif

struct Plugin {
	PluginInfo info;
	void* handle;
};

using MetadataVariant = std::variant<int, bool, std::string, std::vector<std::string>>;

struct Book {
    std::map<std::string, MetadataVariant> metadata; //Arrays should have the char \u001f as separator
    //For calling download links, scraper id  will be in property "scraper_id"
    std::vector<std::string> downloadLinks;
    std::string scraper_id;
};

struct Log {
    std::string msg;
    enum struct LogLevel {
        DEBUG,
        LOG,
        WARN,
        ERROR,
        FATAL,
        APOCALYPSE
    };
    std::string pluginName;
    std::string pluginId;

    std::chrono::zoned_time<std::chrono::seconds> time{std::chrono::current_zone()};
    LogLevel level{Log::LogLevel::LOG};
};

std::vector<Log> logHistory;

Log::LogLevel debugLevel{Log::LogLevel::LOG};
std::ofstream logfile("lastlog.txt", std::ios::binary | std::ios::trunc);

std::vector<Plugin> loadedPlugins;
std::vector<Plugin> scrapers;

static std::map<std::string, Book> searchResults; // ID - Book

static int pluginInit(const PluginInfo* info) {
	/*
    std::cout << "╔══════════════════════════════════════╗\n";
    std::cout << "║ Plugin detected:                     ║\n";
    std::cout << "║  Name:   " << info->name << "\n";
    std::cout << "║  Author: " << info->author << "\n";
    std::cout << "║  Version: " 
              << info->version[0] << "."
              << info->version[1] << "."
              << info->version[2] << "\n";
    std::cout << "║  Type: " << info->type << "                             ║\n";
    std::cout << "╚══════════════════════════════════════╝\n";*/

    Plugin rp;
    rp.info = *info; 
    
    if (info->type == SCRAPER) scrapers.push_back(rp); else loadedPlugins.push_back(rp);

    return 0;
}

void log(const char* message, const char* from, Log::LogLevel level) {
    auto now{std::chrono::system_clock::now()};
    std::chrono::zoned_time local{std::chrono::current_zone(), floor<std::chrono::seconds>(now)};
    std::string levelStr;
    switch (level) {
        case Log::LogLevel::DEBUG: levelStr = "Debug";              break;
        case Log::LogLevel::LOG:   levelStr = "Log";                break;
        case Log::LogLevel::WARN:  levelStr = "Warning";            break;
        case Log::LogLevel::ERROR: levelStr = "Error";              break;
        case Log::LogLevel::FATAL: levelStr = "Fatal error";        break;
        case Log::LogLevel::APOCALYPSE: levelStr = "Apocalypse";    break;
    };
	logfile << std::format("{:%Y/%m/%d - %H:%M.%S}", local) << " : " << levelStr << " from " << from << ": " << message << std::endl;
	if (level >= debugLevel) {
		Log log{
            std::string(message),
            std::string(from),

        };
        logHistory.push_back(log);
	}
}

void debug(const char* message, const char* name)       {log(message, name, Log::LogLevel::DEBUG);}
void print(const char* message, const char* name)       {log(message, name, Log::LogLevel::LOG);}
void  warn(const char* message, const char* name)       {log(message, name, Log::LogLevel::WARN);}
void error(const char* message, const char* name)       {log(message, name, Log::LogLevel::ERROR);}
void fatal(const char* message, const char* name)       {log(message, name, Log::LogLevel::FATAL);}
void apocalypse(const char* message, const char* name)  {log(message, name, Log::LogLevel::APOCALYPSE);}

void addMetadata(const char* bookId, const Metadata data) {
    MetadataVariant value;
    switch (data.tag) {
        case STR: value = std::string(data.value.str); break;
        case INT: value = data.value.num; break;
        case BOOL: value = data.value.b; break;
        case ARRAY: {
            std::string str(data.value.arr);
            std::vector<std::string> arr;
            while (str.find(SEP) != std::string::npos) {
                arr.push_back(str.substr(str.find(SEP)));
                str.erase(str.find(SEP));
            }
            value = arr;
            break;
        }
    }
    searchResults[bookId].metadata[data.key] = value; // -> This really isn't as this. The function will call all AFTER_SEARCH plugins, and then they will return fixed data
    //But, during beta versions, this is enough;
    // (!!) During processing, important to remember arrays (\u001f)
    //debug(("Metadata " + std::string(id) + " for book " + std::string(bookId) + " obtained: " + std::string(value)).c_str(), "Lybra");
}

void setBookScraper(const char* bookId, const char* scraperId) {
    searchResults[bookId].scraper_id = std::string(scraperId);
}

void addDownloadLink(const char* bookId, const char* name) {
    searchResults[bookId].downloadLinks.push_back(name);
    print(("Download link for book " + std::string(bookId) + " obtained: " + std::string(name)).c_str(), "Lybra");
}

bool loadPlugin(const char* path, const bool isScraper) {
	void* lib = LOAD_LIB(path);
    if (!lib) {
        #ifdef _WIN32
            DWORD err = GetLastError();
            std::string msg = "LoadLibrary failed for '" + std::string(path)
                              + "' (GetLastError=" + std::to_string(err) + ")";
            error(msg.c_str(), "Lybra");
        #else
            error(dlerror(), "Lybra");
        #endif
        return false;
    }

    typedef int (*PluginMainFn)(const HostAPI*);
    auto pluginMain = (PluginMainFn)GET_FUNC(lib, "pluginMain");

    if (!pluginMain) {
        error("Invalid plugin", "Lybra");
        FREE_LIB(lib);
        return false;
    }

    HostAPI api;
    api.pluginInit = pluginInit;
    api.debug  = debug;
    api.print  = print;
    api.warn   = warn;
    api.error  = error;
    api.fatal  = fatal;
    api.apocalypse  = apocalypse;

    size_t scrapersBefore = scrapers.size();
    size_t pluginsBefore  = loadedPlugins.size();

    int result = pluginMain(&api);

    for (size_t i = scrapersBefore; i < scrapers.size(); ++i)
        scrapers[i].handle = lib;
    for (size_t i = pluginsBefore; i < loadedPlugins.size(); ++i)
        loadedPlugins[i].handle = lib;

    if (isScraper) {
        typedef int (*GiveAPI)(const ScraperAPI*);
        auto giveApi = (GiveAPI)GET_FUNC(lib, "giveApi");

        if (!giveApi) {
            error("Invalid scraper", "Lybra");
            FREE_LIB(lib);
            return false;
        }

        ScraperAPI sapi;
        sapi.addMetadata = addMetadata;
        sapi.addDownloadLink = addDownloadLink;
        sapi.setBookScraper = setBookScraper;
        result = giveApi(&sapi);
    }

    if (result != 0) {
        error(("PluginMain returned error code " + std::to_string(result)).c_str(), "Lybra");
        FREE_LIB(lib);
        return false;
    }

    return true;
}
