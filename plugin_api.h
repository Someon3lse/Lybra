#pragma once

#ifdef _WIN32
    #define EXPORT_FN __declspec(dllexport)
#else
    #define EXPORT_FN __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif


enum PluginType {
    NONE, //Not automatic plugins. They are called by the user or by other plugins. No preference needed
    SCRAPER,
    AFTER_SEARCH,
    AFTER_DOWNLOAD
};

typedef struct {
    const char* name;
    const char* author;
    int version[3]; // [0]=Major, [1]=Minor, [2]=Patch
    const char* description;
    const char* id; // IDs can be UIDs or short words. Is preferred a UID for non-official plugins
    int preference; //Higher preference vaules are executed before than lower values
    enum PluginType type;
} PluginInfo;


typedef int (*PluginInitFn)(const PluginInfo* info); //All plugins must call this at pluginMain(). It gives main app plugin data
typedef void (*AppLog)(const char* message, const char* pluginName);
typedef void (*GetFn) (const char* pluginId[32], const char* function);

typedef struct {
    PluginInitFn pluginInit;
    AppLog debug;           // -1|  ------  LYBRA INFO  ------  | 
    AppLog print;           // 0 | Users only can see messages  | 
    AppLog warn;            // 1 | with a level equal or higher |
    AppLog error;           // 2 | to debugLevel. DebugLevel is |
    AppLog fatal;           // 3 | set at start of the app and  |
    AppLog apocalypse;      // 4 | it's changed at settings.    |
    int debugLevel;
    GetFn getPluginFn;
} HostAPI;

EXPORT_FN int pluginMain(const HostAPI* api);
EXPORT_FN int pluginShutdown(); //Unused. Now...

#ifdef __cplusplus
}
#endif
