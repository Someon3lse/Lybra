#   ┓   ┓              
#   ┃ ┓┏┣┓┏┓┏┓         
#   ┗┛┗┫┗┛┛ ┗┻
#      ┛      
#  Copyright (C) 2026 Someon3lse
#  This project is under a GPL 3.0 license
#  https://github.com/Someon3lse/Lybra

cmake_minimum_required(VERSION 3.18)

project(Lybra_Desktop LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# set(CMAKE_BUILD_TYPE Debug) Uncomment for Debug

FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
  GIT_TAG v7.0.1
)

FetchContent_MakeAvailable(ftxui)

# === MAIN APP ===
set(HEADERS
    plugin_api.h
    scraper_api.h

    pluginmanager.hpp
    #global.h
    tui.hpp
 )

set(SOURCES 
    pluginmanager.cpp
    tui.cpp
    
    main.cpp
    )

add_executable(Lybra_Desktop 
    ${HEADERS}
    ${SOURCES}
    )

if(MSVC)
    set_property(TARGET Lybra_Desktop PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()


target_link_libraries(Lybra_Desktop PUBLIC ftxui::dom ftxui::component ftxui::screen)

# === HELLO_PLUGIN (C) ===

add_library(hello_plugin SHARED examples/hello_plugin.c)
target_include_directories(hello_plugin PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(MSVC)
    set_property(TARGET hello_plugin PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    target_compile_options(hello_plugin PRIVATE /MD)
endif()

set_target_properties(hello_plugin PROPERTIES
    PREFIX ""
    OUTPUT_NAME "hello_plugin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples"
)

# == RUST_PLUGIN (Rust) ===

find_program(RUSTC_EXE rustc)

if(RUSTC_EXE)
    message(STATUS "rustc found: ${RUSTC_EXE}")

    set(RUST_PLUGIN_SRC  "${CMAKE_CURRENT_SOURCE_DIR}/examples/rust_plugin.rs")
    set(RUST_PLUGIN_OUT  "${CMAKE_BINARY_DIR}/examples/rust_plugin$<IF:$<PLATFORM_ID:Windows>,.dll,.so>")

    add_custom_command(
        OUTPUT  "${RUST_PLUGIN_OUT}"
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_BINARY_DIR}/examples"
        COMMAND "${RUSTC_EXE}"
                --crate-type cdylib
                --edition 2021
                -O
                -o "${RUST_PLUGIN_OUT}"
                "${RUST_PLUGIN_SRC}"
        DEPENDS "${RUST_PLUGIN_SRC}"
        COMMENT "Building rust plugin..."
        VERBATIM
    )

    add_custom_target(rust_plugin ALL DEPENDS "${RUST_PLUGIN_OUT}")

    add_dependencies(Lybra_Desktop rust_plugin)
else()
    message(WARNING "rustc not found")
endif()

include(GNUInstallDirs)
install(TARGETS Lybra_Desktop
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# === EXAMPLE_SCRAPER (C++) ===

add_library(example_scraper SHARED
    examples/scrapers/example_scraper.cpp
    examples/lib/httplib/httplib.h)
target_include_directories(example_scraper PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(example_scraper PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/examples/lib/httplib)
target_include_directories(example_scraper PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/examples/lib/nlohmann)

if(WIN32)
    target_link_libraries(example_scraper PRIVATE ws2_32)
endif()
if(MSVC)
    set_property(TARGET example_scraper PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    target_compile_options(example_scraper PRIVATE /MD)
endif()

set_target_properties(example_scraper PROPERTIES
    PREFIX ""
    OUTPUT_NAME "example_scraper"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples/scrapers"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples/scrapers"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples/scrapers"
)

