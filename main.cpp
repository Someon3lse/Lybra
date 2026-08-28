//   ┓   ┓              
//   ┃ ┓┏┣┓┏┓┏┓         
//   ┗┛┗┫┗┛┛ ┗┻
//      ┛      
//  Copyright (C) 2026 Someon3lse
//  This project is under a GPL 3.0 license
//  https://github.com/Someon3lse/Lybra

#include <iostream>
#include <filesystem>
#include <ftxui/component/screen_interactive.hpp>

#include "pluginmanager.hpp"
#include "tui.hpp"

namespace fs = std::filesystem;

void loadPlugins() {
    fs::path path("./examples");
    try {
        for (const auto& entry : fs::directory_iterator(path)) {
            if (entry.path().extension() == ".dll" || entry.path().extension() == ".so") {
                loadPlugin(entry.path().string().c_str(), false);
                print(std::string("Loading plugin: " + entry.path().filename().string()).c_str(), "Lybra");
            }
        }
    } catch (const fs::filesystem_error& e) {
        error(e.what(), "Lybra");
    }
    path = fs::path("./examples/scrapers");
    try {
        for (const auto& entry : fs::directory_iterator(path)) {
            if (entry.path().extension() == ".dll" || entry.path().extension() == ".so") {
                loadPlugin(entry.path().string().c_str(), true);
                print(std::string("Loading scraper: " + entry.path().filename().string()).c_str(), "Lybra");
            }
        }
    } catch (const fs::filesystem_error& e) {
        error(e.what(), "Lybra");
    } 
}

int main()
{
    // TODO: If Qt6 is available when compiling, don't compile FTXUI, if it isn't, comile FTXUI app
    // (Or if it's specified while compiling, both)
    loadPlugins();
    auto screen{ScreenInteractive::Fullscreen()};
    screenPtr = &screen;

    MainPage mainPage;

    Component page = CatchEvent(mainPage.page(), [&](Event event) {
        if (event == Event::Escape) {
            screen.ExitLoopClosure()(); 
            return true;
        }
        return false;
    });

    screen.Loop(page);
    return 0;
}