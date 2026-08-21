#pragma once

#include <string>
#include <map>
#include <vector>
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

class PageIdle {
public: 
	PageIdle();
	inline const Component& page() {
		return renderer;
	};
private:
    Component renderer;
};

class PageSearch {
public:
	PageSearch();

	void clearBooks();
	void addBook(const string id, const Book book);
	
	inline const Book& currentBook() {
		return selectedBook;
	}

	inline const Component& page() {
		return renderer;
	}
private:
	Component renderer;

	map<string, Book> books;
	Components searchResults;
	Book selectedBook;

	Component bookList{Container::Vertical(searchResults)};
};

class PageMetadata {
public:
	PageMetadata();

	void setBook(const Book& b);
	void addDownloadLink(const string& link);

	inline const Component& page() {
		return renderer;
	}
private:
	Component renderer;

	Book book;
	vector<string> metadataTexts;
	vector<string> fullInfo;
	vector<string> downloadLinks;
	int selectedMetadata = 0;
	Components metadataButtons;
	Components downloadButtons;

	Component metadataContainer{Container::Vertical(metadataButtons)};
	Component downloadContainer{Container::Horizontal(downloadButtons)};
	Component pageMetadataContainer{Container::Vertical({metadataContainer, downloadContainer})};
};

// TODO: make PageSettings
// TODO: Add Modal and Esc = Esc

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
	
	
	Elements logs;

	Component pagesTabs{Container::Tab({pageIdle.page(), pageSearch.page(), pageMetadata.page()}, &currentPage)};
	Component inputQuery{Input(&query, "What do you want to read today?")};
	Component searchButton{Button("Search", [&] () {currentPage = SEARCH;})}; // TODO: Search
	Component inputContainer{Container::Horizontal({inputQuery, searchButton})};
	Component root{Container::Vertical({pagesTabs, inputContainer})};

	Element formatLog(const Log& log);
};