//   ┓   ┓              
//   ┃ ┓┏┣┓┏┓┏┓         
//   ┗┛┗┫┗┛┛ ┗┻
//      ┛      
//  Copyright (C) 2026 Someon3lse
//  This project is under a GPL 3.0 license
//  https://github.com/Someon3lse/Lybra

#include "plugin_api.h"

EXPORT_FN int pluginMain(const HostAPI* api) {
    PluginInfo info;
    info.name        = "Hello Plugin";
    info.author      = "John Dee";
    info.version[0]  = 1;
    info.version[1]  = 0;
    info.version[2]  = 0;
    info.id          = "c_ivilization";
    info.description = "C example plugin";
    info.preference  = -1;
    info.type        = NONE;

    int result = api->pluginInit(&info);
    if (result != 0) return result;

    api->debug("Hello from hello plugin! (｡◕‿‿◕｡)", info.name);

    return 0;
}