#include <thread>

#include "httplib.h"
#include "json.hpp"

#include "scraper_api.h"

using json = nlohmann::json;

const char* pluginId = "thunderhead";
PluginInfo info;

ScraperAPI sapi;
HostAPI hapi;

EXPORT_FN void searchBook(const char* query) {
    hapi.print("Searching book....", "Example Scraper");
    std::thread([query = std::string(query)]() {
        httplib::Client cli("http://openlibrary.org");
        httplib::Headers headers = {
            {"User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36"}
        };
        httplib::Params params = {
            {"q", query}
        };
        hapi.debug("Client created", "Example Scraper");
        if (auto res = cli.Get("/search.json", params, headers)) {
            hapi.debug("Answer received", "Example Scraper");
            json obj = json::parse(res->body);
            for (const auto& j : obj["docs"]) {
                std::string bookId = j.value("key", "unknown-id");
                sapi.setBookScraper(bookId.c_str(), pluginId);
                for (const auto& [key, value] : j.items()) {
                    Metadata metadata;
                    metadata.key = key.c_str();
                    if (value.is_number_integer())  {
                        metadata.value.num = value.get<int>();
                        metadata.tag = INT;
                    }
                    else if (value.is_boolean()) {
                        metadata.value.b = value.get<bool>();
                        metadata.tag = BOOL;
                    }
                    else if (value.is_array()) {
                        std::string arrStr;
                        for (int i = 0; i < value.size(); ++i) {
                            arrStr += value[i].get<std::string>();
                            arrStr += SEP;
                        }
                        metadata.value.arr = arrStr.c_str();
                        metadata.tag = ARRAY;
                    } else {
                        metadata.value.str = value.get<std::string>().c_str();
                        metadata.tag = STR;
                    }
                    sapi.addMetadata(bookId.c_str(), metadata);
                    sapi.addDownloadLink(bookId.c_str(), "No download available");
                }
            }
        } else {
            auto err = res.error();
            hapi.error(("Error: " + httplib::to_string(err)).c_str(), "Example Scraper");
        }
    }).detach();
}

EXPORT_FN int pluginMain(const HostAPI* api) {
    hapi = (*api);
    info.name        = "Example Scraper";
    info.author      = "John Doe III";
    info.version[0]  = 6;
    info.version[1]  = 6;
    info.version[2]  = 6;
    info.description = "An example scraper for obtaining metadata from OpenLibrary (It doesn't download, it only searchs)";
    info.preference  = -1;
    info.id          = pluginId;
    info.type        = SCRAPER; 

    int result = api->pluginInit(&info);
    if (result != 0) return result;

    api->debug("Scraper de ejemplo inicializado correctamente!", info.name);

    return 0;
}
EXPORT_FN int pluginShutdown() {
    return 0;
}
EXPORT_FN int giveApi(const ScraperAPI* api) {
    sapi = (*api);
    return 0;
}

EXPORT_FN void downloadBook(const char* id, const char* name) {
    hapi.warn("Downloading is not functional actually", "Example Scraper");
}

