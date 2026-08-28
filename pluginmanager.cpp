//   ┓   ┓              
//   ┃ ┓┏┣┓┏┓┏┓         
//   ┗┛┗┫┗┛┛ ┗┻
//      ┛      
//  Copyright (C) 2026 Someon3lse
//  This project is under a GPL 3.0 license
//  https://github.com/Someon3lse/Lybra

#include "pluginmanager.hpp"
#include "tui.hpp"

ftxui::ScreenInteractive* screenPtr = nullptr;

std::vector<Log> logHistory;
Log::LogLevel debugLevel{Log::LogLevel::LOG};
std::ofstream logfile("lastlog.txt", std::ios::binary | std::ios::trunc);
std::vector<Plugin> loadedPlugins;
std::vector<Plugin> scrapers;
std::map<std::string, Book> searchResults;
std::mutex resultsMutex;
bool resultsChanged = false;

void refreshUI() {
    if (!resultsChanged) {
        if (screenPtr) screenPtr->PostEvent(Event::Custom);
        resultsChanged = true;
    }
}

static int pluginInit(const PluginInfo* info) {
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
            "", // TODO - Get plugin ID by name, if name not exists, log is  not shown
            local,
            level
        };
        log.time = local;
        logHistory.push_back(log);
        refreshUI();
    }
}

void debug(const char* message, const char* name)       {log(message, name, Log::LogLevel::DEBUG);}
void print(const char* message, const char* name)       {log(message, name, Log::LogLevel::LOG);}
void warn(const char* message, const char* name)        {log(message, name, Log::LogLevel::WARN);}
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
                arr.push_back(str.substr(0, str.find(SEP)));
                str.erase(0, str.find(SEP) + 1);
            }
            if (!str.empty()) arr.push_back(str);
            value = arr;
            break;
        }
    }
    debug(("Metadata " + std::string(data.key) + " obtained for the book " + std::string(bookId)).c_str(), "Lybra");
    {
    std::lock_guard<std::mutex> lock(resultsMutex);
    searchResults[bookId].metadata[data.key] = value;
    }
    refreshUI();
}

void setBookScraper(const char* bookId, const char* scraperId) {
    {
    std::lock_guard<std::mutex> lock(resultsMutex);
    searchResults[bookId].scraper_id = std::string(scraperId);
    }
    refreshUI();
}

void addDownloadLink(const char* bookId, const char* name, const unsigned short server) {
    {
        std::lock_guard<std::mutex> lock(resultsMutex);
        searchResults[bookId].downloadLinks[server] = name;
    }
    debug(("Download link for book " + std::string(bookId) + " obtained: " + std::string(name)).c_str(), "Lybra");
    refreshUI();
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

void searchBook(const char* query) {
    int i = 0;
	for (Plugin scraper : scrapers) {
		void* lib = scraper.handle;
		typedef void (*SearchBook)(const char* query);

		auto searchFn = (SearchBook)GET_FUNC(lib, "searchBook");

		if (!searchFn) {
			error("Scraper without search function", "Lybra");
            FREE_LIB(lib);
            scrapers.erase(scrapers.begin() + i);
            return;
		}

		searchFn(query);
        i++;
	}
}

void downloadBook(const std::string& id, const unsigned short server) {
    int i = 0;
    for (Plugin scraper : scrapers) {

        std::lock_guard<std::mutex> lock(resultsMutex);
        if (scraper.info.id != searchResults.at(id).scraper_id) {
         continue; i++;
        }
        void* lib = scraper.handle;
        typedef void (*DownloadBook)(const char* bookId, const unsigned short downloadName);

        auto downFn = (DownloadBook)GET_FUNC(lib, "downloadBook");

        if (!downFn) {
            error("Scraper without download function", "Lybra");
            FREE_LIB(lib);
            scrapers.erase(scrapers.begin() + i);
            return;
        }

        downFn(id.c_str(), server); 
        break;
    }
}