//   ┓   ┓              
//   ┃ ┓┏┣┓┏┓┏┓         
//   ┗┛┗┫┗┛┛ ┗┻
//      ┛      
//  Copyright (C) 2026 Someon3lse
//  This project is under a GPL 3.0 license
//  https://github.com/Someon3lse/Lybra

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <chrono>
#include <format>
#include <variant>
#include <mutex>
#include <ftxui/component/screen_interactive.hpp>

#include "scraper_api.h" 

#ifdef _WIN32
    #include <windows.h>
    #define LOAD_LIB(path)       LoadLibraryA(path)
    #define GET_FUNC(lib, name)  GetProcAddress((HMODULE)lib, name)
    #define FREE_LIB(lib)        FreeLibrary((HMODULE)lib)
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
    #define NOGDI
#else
    #include <dlfcn.h>
    #define LOAD_LIB(path)       dlopen(path, RTLD_LAZY)
    #define GET_FUNC(lib, name)  dlsym(lib, name)
    #define FREE_LIB(lib)        dlclose(lib)
#endif


extern ftxui::ScreenInteractive* screenPtr;

struct Plugin {
	PluginInfo info;
	void* handle;
};

using MetadataVariant = std::variant<int, bool, std::string, std::vector<std::string>>;

struct Book {
    std::map<std::string, MetadataVariant> metadata;
    //For calling download links, scraper id  will be in property "scraper_id"
    std::map<unsigned short, std::string> downloadLinks; // Server, text
    std::string scraper_id;
};

struct Log {
    std::string msg;
    std::string pluginName;
    std::string pluginId;

    std::chrono::zoned_time<std::chrono::seconds> time{std::chrono::current_zone()};


    enum struct LogLevel {
        DEBUG,
        LOG,
        WARN,
        ERR,
        FATAL,
        APOCALYPSE
    };

    LogLevel level{Log::LogLevel::LOG};
};

extern std::vector<Log> logHistory;
extern Log::LogLevel debugLevel;
extern std::ofstream logfile;
extern std::vector<Plugin> loadedPlugins;
extern std::vector<Plugin> scrapers;
extern std::map<std::string, Book> searchResults; // ID - Book
extern std::mutex resultsMutex;
extern bool resultsChanged;

void refreshUI();

//Plugin functions 

static int pluginInit(const PluginInfo* info);

void log(const char* message, const char* from, Log::LogLevel level);
void debug(const char* message, const char* name);
void print(const char* message, const char* name);
void warn(const char* message, const char* name);
void error(const char* message, const char* name);
void fatal(const char* message, const char* name);
void apocalypse(const char* message, const char* name);

void addMetadata(const char* bookId, const Metadata data);
void setBookScraper(const char* bookId, const char* scraperId);
void addDownloadLink(const char* bookId, const char* name, const unsigned short server);

// Main app functions 

bool loadPlugin(const char* path, const bool isScraper);
void searchBook(const char* query);
void downloadBook(const std::string& id, const unsigned short server);