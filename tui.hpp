#pragma once

#include <string>
#include <map>
#include <vector>
#include <set>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>

#include "pluginmanager.hpp" 

using namespace ftxui; 
using namespace std;


enum pages {IDLE, SEARCH, METADATA, SETTINGS};
static int currentPage = IDLE;
static ButtonOption btnOpt;

static string modalText;
static bool modalVisible;

static bool showTime = true;
static int maxLogs = 4;

static std::string selectedBookId{""};

class PageIdle {
public: 
	PageIdle();
	inline const Component& page() {
		return renderer;
	};
private:
	Component settingsButton{Button("Settings", []{}, btnOpt)};
	Component infoButton{Button("About", []{}, btnOpt)};
	Component closeButton{Button("Exit", []{
		screenPtr->ExitLoopClosure()();
	}, btnOpt)};
	Components buttons{settingsButton, infoButton, closeButton};
	Component buttonContainer{Container::Horizontal(buttons)};
    Component renderer;
};

class PageSearch {
public:
	PageSearch();

	void clearBooks();

	inline const Component& page() {
		return renderer;
	}
private:
	Component renderer;

	map<string, Book>& books{searchResults};

	Component bookList{Container::Vertical({})};
};

class PageMetadata {
public:
	PageMetadata();

	void setBook(const Book& b);
	void addDownloadLink(const unsigned short server);

	inline const Component& page() {
		return renderer;
	}
private:
	Component renderer;

	Book book;
	vector<string> metadataTexts;
	vector<string> fullInfo;
	map<unsigned short, string> downloadLinks;
	int selectedMetadata = 0;
	Components metadataButtons;
	Components downloadButtons;

	Component metadataContainer{Container::Vertical(metadataButtons)};
	Component downloadContainer{Container::Horizontal(downloadButtons)};
	Component pageMetadataContainer{Container::Vertical({metadataContainer, downloadContainer})};
};

// TODO: make PageSettings

class MainPage {
public:
	MainPage();

	inline const Component& page() {
		return renderer;
	}
private:
	Component renderer;

	PageIdle pageIdle;
   	PageSearch pageSearch;
   	PageMetadata pageMetadata;

	std::string query;

	std::string placeholder;
	
	
	Elements logs;

	Component pagesTabs{Container::Tab({pageIdle.page(), pageSearch.page(), pageMetadata.page()}, &currentPage)};
   	Component closeModal{Button("Close", [&]{ modalVisible = false; })};
	Component inputQuery{Input(&query, &placeholder)};
	Component searchButton{Button("Search", [&] () {pageSearch.clearBooks(); currentPage = SEARCH; searchBook(query.c_str()); })}; // TODO: Search
	Component inputContainer{Container::Horizontal({inputQuery, searchButton})};
	Component root{Container::Vertical({pagesTabs, inputContainer})};
	Component modal{Renderer(closeModal, [&] {
        return vbox({
            text(modalText) | center,
            separator(),
            closeModal->Render() | center,
        }) | border;
    })};

	Element formatLog(const Log& log);
};

//FIX: It seems like search results, when changed, don't change search Page... 