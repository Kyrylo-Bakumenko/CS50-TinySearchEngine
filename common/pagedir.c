/* pagedir.c    Kyrylo Bakumenko    23 April, 2023
 *
 * This file contains methods for initalizing a directory
 * and saving crawled files found through a call to crawler.c
 * 
 * Error Codes: 1 -> Cannot write to specified directory
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../libcs50/webpage.h"
#include "../libcs50/mem.h"

// function prototypes
bool pagedir_init(const char* pageDirectory);
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);

/**************** pagedir_init() ****************/
/* See pagedir.h for more information           */
bool 
pagedir_init(const char* pageDirectory)
{
    // construct the pathname for the .crawler file in that directory
    FILE *fp = NULL;
    const char* crawler = "/.crawler";
    char* crawler_path = mem_malloc(strlen(pageDirectory) + strlen(crawler) + 1);
    strcpy(crawler_path, pageDirectory);
    strcat(crawler_path, crawler);
    
    // open the file for writing; on error, return false.
    if ((fp = fopen(crawler_path, "w")) == NULL) {
        fprintf(stderr, "ERROR: Cannot write to %s\n", crawler_path);
        mem_free(crawler_path);
        return false;
    }
    // close the file and return true.
    fclose(fp);
    mem_free(crawler_path);
    return true;
}

/**************** pagedir_save() ****************/
/* See pagedir.h for more information           */
void 
pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID)
{
    // construct the pathname for the page file in pageDirectory
    FILE *fp = NULL;
    char suffix[20];
    sprintf(suffix, "/%d", docID);
    char* page_path = mem_malloc(strlen(pageDirectory) + strlen(suffix) + 1);
    strcpy(page_path, pageDirectory);
    strcat(page_path, suffix);

    // open the file for writing; on error, return 1.
    if ((fp = fopen(page_path, "w")) == NULL) {
        fprintf(stderr, "ERROR: Cannot write to %s", page_path);
        mem_free(page_path);
        exit(1);
    }
    // print the URL
    fprintf(fp, "%s\n", webpage_getURL(page));
    // print the depth
    fprintf(fp, "%d\n", webpage_getDepth(page));
    // print the contents of the webpage
    fprintf(fp, "%s\n", webpage_getHTML(page));
    // close the file and return true.
    fclose(fp);
    mem_free(page_path);
}
