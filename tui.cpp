#include "tui.hpp"

PageIdle::PageIdle() {
	renderer = Renderer([] {
	    auto Red {color(Color::Red)};
	    auto Blue{color(Color::Blue)};
	    auto Cyan{color(Color::Cyan)};
	    return vbox({
	        hbox({text("      "), text("▄▄▀▀▀▀▄▄") | Red, text("     "),
                text("▀▀█") | Blue, text("        "),
                text("▀▀█") | Blue, text("  ")}),

	        hbox({text("    "), text("▄▀") | Red, text("        "), text("▀▄") | Red,
	            text("     "), text("█") | Blue, text("          "), text("█") | Blue}),

	        hbox({text("   "), text("▄▀") | Red, text("          "), text("▀▄") | Red,
	            text("    "), text("█") | Blue, text("          "), text("█") | Blue}),

	        hbox({text("   "), text("█") | Red, text("           "), text("▄█") | Red,
	             text("    "), text("█") | Blue, text("          "), text("█") | Blue}),

	        hbox({text("    "), text("█") | Red, text("          "), text("█") | Red,
	             text("     "), text("█") | Blue, text("          "), text("█") | Blue}),

	        hbox({text("     "), text("▀▄") | Red, text("      "), text("▄▀") | Red,
	            text("      "), text("█") | Blue, text("   "), text("▀█") | Blue,
	            text("  "), text("█▀") | Blue, text(" "), text("█▀▀█") | Blue,
	            text(" "), text("█▀▀█") | Blue, text(" "), text("█▀▀█") | Blue}),

	        hbox({text("  "), text("▀▀▀▀▀") | Red, text("      "), text("▀▀▀▀▀") | Red,
	            text("   "), text("█") | Blue, text("  "), text("▄") | Blue,
	            text(" "), text("█") | Blue, text("  "), text("█") | Blue,
	            text("  "), text("█") | Blue, text("  "), text("█") | Blue,
	            text(" "), text("█") | Blue, text(""),
	            text("    "), text("█") | Blue, text("  "), text("█") | Blue}),

	        hbox({text("  "), text("▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀") | Red, text("   "),
	            text("▀▀▀▀") | Blue, text(" "), text("▀▀▀█") | Blue, text("  "),
	            text("▀▀▀▀") | Blue, text(" "), text("▀") | Blue, text("    "),
	            text("▀▀▀▀▀") | Blue}),

	        hbox({text("  "), text("█████████████████████████████████████████████") | Cyan}),

	        hbox({text("                          "),
	            text("█  █") | Blue}),

	        hbox({text("  "), text("███████████████████████████") | Cyan,
	            text("█") | Blue, text("█████████████████") | Cyan}),

	        hbox({text("                          "),
	            text("▀▀▀▀") | Blue}),

	    }) | hcenter | flex;
	});
}

PageSearch::PageSearch() {
	renderer = Renderer(bookList, [&] {
    return vbox({
        text("Select book: ") | bold,
        filler(),
        bookList->Render() | vscroll_indicator | yframe | flex
        }) | border;
    });
}

void PageSearch::clearBooks() {
	searchResults.clear();
	selectedBook.metadata.clear();
	selectedBook.downloadLinks.clear();
	selectedBook.scraper_id = "";
}

void PageSearch::addBook(const string id, const Book book) {
	books[id] = book;
	MetadataVariant title = book.metadata.at("title");
	MetadataVariant author = book.metadata.at("author_names");
	string titleStr;
	string authorStr;
	if(holds_alternative<string>(title)) titleStr = get<string>(title);
	else titleStr = "[Unknown title]";

	if(holds_alternative<vector<string>>(author)) {
		vector<string>& authorVec = get<vector<string>>(author);
		if (authorVec.size() > 1) authorStr = authorVec.at(0) + " and " + to_string(authorVec.size() - 1) + " more...";
		else authorStr = "[Unknown author]";
	}

	searchResults.push_back(Button(
    titleStr + ", by " + authorStr, [&, id] {
    	selectedBook = books[id];
        currentPage = METADATA;
    }, btnOpt));
}

PageMetadata::PageMetadata() {
	renderer = Renderer(pageMetadataContainer, [&] {
    	return vbox({
        text("Metadata of " + get<string>(book.metadata.at("title"))) | bold,
        filler(),
        metadataContainer->Render() | vscroll_indicator | yframe | flex,
        separator(),
        downloadContainer->Render() | hscroll_indicator | xframe | flex | size(HEIGHT, GREATER_THAN, 4)
        }) | border;
	});
}

void PageMetadata::setBook(const Book& b) {
	book = b;
	metadataTexts.clear();
	fullInfo.clear();
	selectedMetadata = 0;
	map<string, MetadataVariant>& metadata = book.metadata;

	for (const auto& [key, value] : metadata) {
    	string text{key};
    	while (text.find("_") != string::npos)
        	text[text.find("_")] = ' ';
   		text[0] = toupper(static_cast<unsigned char>(text[0]));
    
        string fullText{text};
        text += ": ";
        fullText += "\n";
        fullText += "Original key: " + key + "\n";
        fullText += "Value(s)\n";

        if (holds_alternative<vector<string>>(value)) {
            if (get<vector<string>>(value).size() == 1) text += get<vector<string>>(value).at(0);
            else text += get<vector<string>>(value).at(0) + " [View all]";
            for (string str : get<vector<string>>(value))
            fullText += " · " + str + "\n";
        } else if (holds_alternative<int>(value)) {
            text += to_string(get<int>(value));
            fullText += to_string(get<int>(value));
        } else if (holds_alternative<string>(value)) {
            text += get<string>(value);
            fullText += get<string>(value);
        } else if (holds_alternative<bool>(value)) {
            text += get<bool>(value) ? "true" : "false";
            fullText += get<bool>(value) ? "true" : "false";
        }

        metadataTexts.push_back(text);
        fullInfo.push_back(fullText);
    }

    for (int i = 0; i < metadataTexts.size(); ++i) {
        auto buttonMetadata{Button(metadataTexts[i], [&, i]{
            selectedMetadata = i;
           	modalText = fullInfo[i];
           	modalVisible = true; // TODO: Add modal
        }, btnOpt)};
        metadataButtons.push_back(buttonMetadata);
    };
}

void PageMetadata::addDownloadLink(const string& link) {
	auto buttonDownload{Button(link, [&, link] {/*Download the book*/})};
	downloadButtons.push_back(buttonDownload);
}

MainPage::MainPage() {
	btnOpt.transform = [](const EntryState& state){
        Element e = text((state.active ? " ▶ " : "- ") + state.label);
        if (state.active) {
            e |= bold;
            e |= focus;
            e |= inverted;
        }
        return e;
	};

	renderer = Renderer(root, [&] {
        Elements logs;

        for (int i = 1; i <= min<size_t>(maxLogs, logHistory.size()); i++) {
            logs.push_back(formatLog(logHistory[logHistory.size() - i]));
        }
        return window(
            text("♎ Lybra") | hcenter | bold,
            vbox({
                filler(),

                pagesTabs->Render() | flex,
                separator(),


                hbox({
                    inputQuery->Render() | border | flex,
                    text(" "),
                    searchButton->Render()
                }),

                vbox(logs) | flex | size(HEIGHT, LESS_THAN, maxLogs),
            })
        ) | flex;
    });
}

Element MainPage::formatLog(const Log& log) {
	string  msgString;
    if (showTime) {
        msgString += format("{:%H:%M.%S}", log.time) + " ";
    }
    msgString += "[" + string(log.pluginName) + "] " + string(log.msg);
    auto logText = text(msgString);
    switch (log.level) {
        case Log::LogLevel::DEBUG: 	logText |= color(Color::Green);      break;
        case Log::LogLevel::WARN:  	logText |= color(Color::Yellow);     break;
        case Log::LogLevel::ERROR: 	logText |= color(Color::Red);        break;
        case Log::LogLevel::FATAL: 	logText |= color(Color::Red) | bold; break;
        case Log::LogLevel::APOCALYPSE: logText |= color(Color::Red) | bold | inverted; break;
        default: 					logText |= color(Color::White); break;
    }
    return logText;
};