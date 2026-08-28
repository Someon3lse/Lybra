//   ┓   ┓              
//   ┃ ┓┏┣┓┏┓┏┓         
//   ┗┛┗┫┗┛┛ ┗┻
//      ┛      
//  Copyright (C) 2026 Someon3lse
//  This project is under a GPL 3.0 license
//  https://github.com/Someon3lse/Lybra

#pragma once

#include "plugin_api.h"

#ifdef __cplusplus
//Import notice: 
// Book metadata should follow Open Library metadata schema (with some modifications) for being correctly processed
// An example can be found in exampleBook.json
// Camel case is preferred for processing keys

extern "C" {
#endif

// Array separator
#define SEP "\x1F"
#define SEP_CHAR '\x1F'

typedef enum {
	STR, INT, BOOL, ARRAY
} Tag;

typedef struct {
	const char* key;
	union {
		const char* str;
		int num;
		bool b;
		const char* arr; 
	} value;
	int tag;
} Metadata;

typedef void (*AddMetadata)(const char* bookId, const Metadata data); 
typedef void (*AddDownload)(const char* bookId, const char* name, const unsigned short server);
typedef void (*SetScraper) (const char* bookId, const char* scraperId);

typedef struct {
	AddMetadata addMetadata;
	AddDownload addDownloadLink;
	SetScraper  setBookScraper;
} ScraperAPI;

EXPORT_FN int giveApi(const ScraperAPI* api);
EXPORT_FN void searchBook(const char* query);
EXPORT_FN void downloadBook(const char* id, const unsigned short server); //Book ID, Download Server

}
