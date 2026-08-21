#include "plugin_api.h"

#ifdef __cplusplus
//Import notice: 
// Book metadata should follow Open Library metadata schema (with some modifications) for being correctly processed
// An example can be found in exampleBook.json
// Camel case is preferred for processing keyss

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

typedef void (*AddMetadata)(const char* bookId, const Metadata data); // Adapt
typedef void (*AddDownload)(const char* bookId, const char* name);
typedef void (*SetScraper) (const char* bookId, const char* scraperId);

typedef struct {
	AddMetadata addMetadata;
	AddDownload addDownloadLink;
	SetScraper  setBookScraper;
} ScraperAPI;

EXPORT_FN int giveApi(const ScraperAPI* api);
EXPORT_FN void searchBook(const char* query);
EXPORT_FN void downloadBook(const char* id, const char* name); //Book ID, Download Link

}
