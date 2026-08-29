> [!CAUTION]
> Fight for your phone! [#KeepAndroidOpen](https://keepandroidopen.org)

# Lybra
> [!IMPORTANT]
> This application is provided **"as is"**, strictly for educational, informational, or research purposes.
The developer assumes **no responsibility** for any use made by third parties of this application, nor for the consequences arising from such use. It is strictly prohibited to use this software to infringe upon intellectual property laws, copyrights, or any other applicable local or international legislation.  
The user assumes full responsibility for their use of the application. It is the sole **responsibility of the user** to ensure that their use complies with applicable laws and respects the copyrights of third parties. The developer does not encourage, support, or facilitate piracy or the illegal use of protected content

> [!TIP]
> Another thing: you probably won't read or do something with the upper information, but you should know this. If you do something with my app, it's **YOUR** problem.  
> If you want to use this for piracy, do it, but I'm not responsible. After that... Feel free to use this how you want! 
> ψ(｀∇´)ψ

## What's this ?

(ʘ‿ʘ)╯ <(Welcome!)   

Lybra is an app for downloading books from various sites.  
It's based in a plugin system, so you can add or remove origins if you want.  

Enjoy reading!

> [!NOTE]
> Lybra doesn't  come with any installed plugins. I'm working in a marketplace and a list of default plugins.
It's actually an alpha release, so it might have errors.
It's under a [GPL 3.0 license](LICENSE)

[¬º-°]¬ <(BOOKS...)  

## ToDos
_But not GlaDos ٩(^‿^)۶_

Lybra is under v1.0.0 version. I have this list for future versions. Feel free to help! (✿◠‿◠)  
- [ ] More scrapers (see [FMHY](https://fmhy.pages.dev/reading) for a full list of objectives)  
- [ ] Remove duplicates in search results   
- [ ] Add execution times for the rest of plugins  
- [ ] GUI (Qt) & GUI plugins (QML) [^1]
- [ ] CLI without needing a TUI  (something like lybra --search "Harry Potter" --download EPUB)
- [ ] Books conversion  
- [ ] Metadata repair  
- [ ] Library tracker (SLUM ?)  
- [ ] Android version (future branch `android`) & WASM version (future branch `web`) Note: this versions should have scrapers already compiled, because is harder to have a plugin system there
- [ ] Settings page
- [ ] Marketplace
- [ ] A promotional web [^2]
- [ ] Book reader

I don't have a Mac or an iPhone, but, if you have one, please consider help building for those platforms! 
(Normal building should work in Mac, see [building](#Building))  

Any ideas? Say them on **issues** page!  

( ͡° ͜ʖ ͡°) Lenny will work on them!

## Usage

Lybra includes an easy to use TUI for fast searching and downloading books.
In your terminal, write `sudo ./Lybra_Desktop` or `Lybra_Desktop` and use it!

## Building

Lybra's dependencies are header-only or are installed in CMakeLists. So, with a C/C++ compiler and inside source code folder, do:
```
mkdir build && cd build
cmake ..
make
```
This will compile Lybra and example plugins
If you have installed rustc, this will build also rust_plugin

## Plugin creation

For creating a custom plugin or scraper, just include `plugin_api.h` or `scraper_api.h` in your plugin, and override functions using C types in your code. Plugins should function in al languages with a C binding, like Zig, C, C++, Rust, C#, Python (Cython), etc.  

Inside plugins, pugins should store HostAPI of pluginMain() and ScraperAPI of giveAPI(). These functions are int for returning errors in complex plugins, like dependencies. Also, plugins must call pluginInit() for being stored.  

Plugin communication and more types of plugins are a work in progress.  
Plugins and scrapers are searched at `./examples/` and `./examples/scrapers/` directories, so, DLLs / SOs should be added there

## Dependencies

Lybra uses [FTXUI](https://github.com/ArthurSonzogni/FTXUI) (MIT, Copyright (c) 2019 Arthur Sonzogni)  
Example Scraper uses [Cpp-Httplib](https://github.com/yhirose/cpp-httplib) (MIT, Copyright (c) 2017 yhirose) and [nlohmann/json](https://github.com/nlohmann/json) (MIT, Copyright (c) 2013-2026 Niels Lohmann)  

All licenses are fully compatible with Lybra's GPL-3.0  
---
Do you like Lybra? Please give a star to the project!

---

(ˆ⺫ˆ๑)<3 Wooops!

[^1]: Qt is preferred because it has official support for WASM, other frameworks don't support this (FTXUI is also preferred because that)  
[^2]: I was working for adding text/qml to IANA supported media types, and, after that, making it a GHPages supported type... 
