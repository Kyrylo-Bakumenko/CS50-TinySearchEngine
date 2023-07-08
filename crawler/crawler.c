/* crawler.c    Kyrylo Bakumenko    23 April, 2023
 *
 * This file accepts 3 arguments
 * Accepts internal url, existing directory, and max depth int parameters
 * Performs dfs search (__crawl__) for internal links on a given url (__pageScan__).
 * found pages are added to the given directory through __pagedir_save__.
 * 
 * Exit codes: 1 -> invalid number of arguments
 *           : 2 -> one or multiple arguments are null
 *           : 3 -> given url is not internal
 *           : 4 -> maximum depth passed is out of range [0, 10]
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "pagedir.h"
#include "webpage.h"
#include "hashtable.h"
#include "bag.h"
#include "mem.h"

// internal function prototypes
static void parseArgs(const int argc, char* argv[], char** seedURL, char** pageDirectory, int* maxDepth);
static void crawl(char* seedURL, char* pageDirectory, const int maxDepth);
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen);
static void logr(const char* word, const int depth, const char* url);

/* ***************************
 *  main function
 *  Accepts 3 arguments
 *  evokes parseArgs and crawl method
 */
int main(int argc, char *argv[])
{
    char* seedURL = NULL;
    char* pageDirectory = NULL;
    int maxDepth = 0;
    parseArgs(argc, argv, &seedURL, &pageDirectory, &maxDepth);
    crawl(argv[1], argv[2], maxDepth);

    return 0; // exit status
}

/**************** crawl() ****************/
/* accepts internal url, existing directory, and max depth int parameters */
/* this function performs a dfs search for internal links on a given url, */
/* found pages are added to the given directory through __pagedir_save__  */
static void 
crawl(char* seed, char* pageDirectory, const int maxDepth)
{
    char* seedURL = mem_malloc(strlen(seed) + 1);
    strcpy(seedURL, seed);

    // initialize hashtable (size=200, assume collisions)
    hashtable_t* pagesSeen = hashtable_new(200);

    // add seedURL
    hashtable_insert(pagesSeen, seedURL, "");

    // initialize the bag and add a webpage representing the seedURL at depth 0
    bag_t* pagesToCrawl = bag_new();
    webpage_t* seedPage = webpage_new(seedURL, 0, NULL);
    bag_insert(pagesToCrawl, seedPage);

    // docID counter
    int docID = 0;

    // while there are more webpages in the bag: extract a page
    webpage_t* curPage = NULL;
    while ((curPage = (webpage_t*) bag_extract(pagesToCrawl)) != NULL) {
        // Sleep for one second if succesful
        sleep(1);
        // fetch url; page->html = NULL at call and page->html = html_content after 
        // if succesful, 
        if (webpage_fetch(curPage)) {
            // log fetch
            logr("Fetched", webpage_getDepth(curPage), webpage_getURL(curPage));
            // save the webpage to pageDirectory
            pagedir_save(curPage, pageDirectory, ++docID);
		    // if the webpage is not at maxDepth,
            if (webpage_getDepth(curPage) < maxDepth) {
                // pageScan that HTML
                // log scan
                logr("Scanning", webpage_getDepth(curPage), webpage_getURL(curPage));
                pageScan(curPage, pagesToCrawl, pagesSeen);
            }
        }
        // delete webpage object attached to curPage
        webpage_delete(curPage);
    }

    hashtable_delete(pagesSeen, NULL);
    bag_delete(pagesToCrawl, webpage_delete);
}

/**************** pageScan() ****************/
/* recursively searches a given webpage for internal links in DFS approach */
/* the source page is added to the hashtable pagesSeen and the found links */
/* are added to pagesToCrawl */ 
static void
pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen)
{
    // current depth
    int curDepth = webpage_getDepth(page);
    // pointer to position in html
    int pos = 0;
    // get URL
    char* nextUrl = NULL;
    // while URL in the page is not null
    while ((nextUrl = webpage_getNextURL(page, &pos)) != NULL) {
        // normalize
        char* normURL = normalizeURL(nextUrl);
        // log found
        logr("Found", webpage_getDepth(page), normURL);
        // if internal, try to insert webpage into hashtable
        if (isInternalURL(normURL)) {
            // verify that the url has not been scanned
            if (hashtable_insert(pagesSeen, normURL, "")) { 
                // if succesful, create a webpage_t and insert into bag
                // log added
                logr("Added", webpage_getDepth(page), normURL);
                webpage_t* newPage = webpage_new(normURL, curDepth+1, NULL);
                bag_insert(pagesToCrawl, newPage);
            } else {
                // log IgnDupl
                logr("IgnDupl", webpage_getDepth(page), normURL);
                // free the norm URL
                mem_free(normURL);
            }
        } else {
            // log IgnExtrn
            logr("IgnExtrn", webpage_getDepth(page), normURL);
            // free the norm URL
            mem_free(normURL);
        }
        // free the URL
        mem_free(nextUrl);

    }
    // add url to hashtable
    hashtable_insert(pagesSeen, webpage_getURL(page), "");
        
}
/**************** parseArgs() ****************/
/* this method assures that passed arguments to crawl are valid */
/* expectations and assumptions are enumerated in the README.md */
static void 
parseArgs(const int argc, char* argv[], char** seedURL, char** pageDirectory, int* maxDepth)
{
    // check num args
    if (argc != 4) {
        fprintf(stderr, "ERROR: Expected 3 arguments but recieved %d", argc-1);
        exit(1);
    }
    // Defensive programming
    if (argv[1] == NULL || argv[2] == NULL || argv[3] == NULL) {
        fprintf(stderr, "ERROR: NULL argument passed");
        exit(2);
    }
    // assign variables
    seedURL = &argv[1];
    pageDirectory = &argv[2];
    *maxDepth = atoi(argv[3]);

    // normlaize URL
    char* normURL = normalizeURL(*seedURL);
    // validate that it is internal
    if (!isInternalURL(normURL)) {
        fprintf(stderr, "ERROR: Non-internal URL %s", normURL);
        mem_free(normURL);
        exit(3);
    }
    mem_free(normURL);
    if (pagedir_init(*pageDirectory)) {
        // if succesful, assert that maxDepth is within range
        if (*maxDepth < 0 || *maxDepth > 10) {
            fprintf(stderr, "ERROR: maxDepth, %d, is out of range [0, 10]!", *maxDepth);
            exit(4);
        }
    }

}
/**************** logr() ****************/
/* this function logs the actions of __crawl__ and __pageScan__ */
/* namely when a page is Found, Added, or Ignored (duplicate or */ 
/* not internal), as well as when HTML information is Fetched   */
static void logr(const char* word, const int depth, const char* url)
{
    printf("%2d %*s%9s: %s\n", depth, depth, "", word, url);
}
