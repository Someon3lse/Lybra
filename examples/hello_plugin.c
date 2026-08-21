#include "plugin_api.h"

EXPORT_FN int pluginMain(const HostAPI* api) {
    PluginInfo info;
    info.name        = "Hello Plugin";
    info.author      = "Geoffrey Epp-Steintz";
    info.version[0]  = 1;
    info.version[1]  = 0;
    info.version[2]  = 0;
    info.id          = "narnia";
    info.description = "Un plugin de prueba";
    info.preference  = -1;
    info.type        = NONE;

    int result = api->pluginInit(&info);
    if (result != 0) return result;

    api->debug("Plugin Hello inicializado correctamente!", info.name);

    return 0;
}

//Build with: gcc -shared -fPIC -o hello_plugin.so hello_plugin.c
// (Windows: cl /LD /O2 /MD /DNDEBUG hello_plugin.c /link /DEBUG:NONE /Fehello_plugin.dll)
