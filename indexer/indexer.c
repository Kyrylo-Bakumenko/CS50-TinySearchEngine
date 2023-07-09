/* indexer.c    Kyrylo Bakumenko    23 April, 2023
 *
 * This file implements the indexer module for the TSE.
 * From a specified crawler output directory, pageDirectory,
 * indexer reads all output files and creates an index with
 * word keys, and a counter item stating how many times the word
 * has appeared in that specific file.
 * 
 * Exit codes: 1 -> invalid number of arguments
 *             2 -> one or more arguments are null
 *             3 -> directory path is not valid
 *             4 -> provided directory is not a crawler directory
 */

#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include "index.h"
#include "webpage.h"
#include "file.h"
#include "mem.h"

// internal function prototypes
static void indexBuild(index_t* index, char* pageDirectory);
static void indexPage(index_t* index, webpage_t* page, int docID);

/* ***************************
 *  main function
 *  Accepts 2 arguments: (pageDirectory indexFilename)
 *  creates an index from pageDirectory
 *  writes inverted index into indexFilenmae
 */
int main(int argc, char *argv[])
{
    /* parse the command line, validate parameters */ 
    // check num parameters
    if (argc != 3) {
        fprintf(stderr, "ERROR: Expected 2 arguments but recieved %d\n", argc-1);
        exit(1);
    }
    // Defensive programming
    if (argv[1] == NULL || argv[2] == NULL) {
        fprintf(stderr, "ERROR: NULL argument passed\n");
        exit(2);
    }


    /* initialize other modules */

    index_t* index = NULL;

    /* call indexBuild, with pageDirectory */
    char* pageDirectory = argv[1];
    char* indexFilename = argv[2];
    /* creates a new 'index' object */ 
    index = index_new(200);
    indexBuild(index, pageDirectory);

    /* create a file indexFilename and write the index to that file, in the format described below. */
    index_save(index, indexFilename);
    index_delete(index);
    mem_free(index);


    return 0; // exit status
}

/**************** indexBuild() ****************/
/* For every output file from pageDirectory (crawled dircetory),    */
/* extracts url, depth, and html data, creates a webpage            */
/* pass this into indexPage for indexing of word data               */
static void
indexBuild(index_t* index, char* pageDirectory)
{
    // ASSUMPTION, docID is less than 10^19, this is a purposeful overestimate of the number of files possible in a directory 
    int docID = 1;
    static int const DOC_ID_MAX_LEN = 20;
    char docString[DOC_ID_MAX_LEN];
    char* filePath = mem_malloc(strlen(pageDirectory) + DOC_ID_MAX_LEN + 1);
    FILE* fp;
    webpage_t* page;
    // check if this is a crawler directory
    strcpy(filePath, pageDirectory);
    DIR* dir = opendir(filePath);
    if (dir) {
        /* Directory exists. */
        closedir(dir);
    } else {
        /* directory does not exist */
        fprintf(stderr, "ERROR: directory %s does not exist!\n", pageDirectory);
        mem_free(filePath);
        exit(3);
    }
    strcat(filePath, "/.crawler");
    if ((fp = fopen(filePath, "r")) == NULL) {
        fprintf(stderr, "ERROR: Crawler directory %s not found!\n", pageDirectory);
        mem_free(filePath);
        exit(4);
    }
    fclose(fp);

    // initialize filePath from directory and docID
    strcpy(filePath, pageDirectory);
    strcat(filePath, "/");
    sprintf(docString, "%d", docID);
    strcat(filePath, docString);

    /* loops over document ID numbers, counting from 1               */
    while ((fp = fopen(filePath, "r")) != NULL) {
        /* loads a webpage from the document file 'pageDirectory/id' */
        // find number of chars in file
        // assumption, less than that of MAX_INT
        int numChars = 0;
        while (fgetc(fp) != EOF) {
            numChars++;
        }
        // rewind stream to start
        rewind(fp);

        // save url from file
        char* url;
        if (!feof(fp)) {
            url = file_readLine(fp);
            if (url == NULL) {
                // variable cleanup
                mem_free(filePath);
                fclose(fp);
                exit(1);
            }
            // dont mem_free, should be cleared by webpage
        }
        // save depth from file
        int depth;
        char* line;
        if (!feof(fp)) {
            line = file_readLine(fp);
            if (line == NULL) {
                // variable cleanup
                mem_free(filePath);
                fclose(fp);
                exit(1);
            }
            sscanf(line, "%d", &depth);
            mem_free(line);
        }
        // save HTML from file
        char* html = mem_malloc(numChars - strlen(url) + 1);
        strcpy(html, "");
        while (!feof(fp)) {
            line = file_readLine(fp);
            if (line != NULL) {
                strcat(html, line);
                mem_free(line);
            } 
        }

        // generate webpage_t from URL, depth, and HTML
        page = webpage_new(url, depth, html);
        /* if successful, passes the webpage and docID to indexPage */
        indexPage(index, page, docID);
        // delete webpage
        webpage_delete(page);

        // loop to next docID
        docID++;
        strcpy(filePath, pageDirectory);
        strcat(filePath, "/");
        sprintf(docString, "%d", docID);
        strcat(filePath, docString);
        fclose(fp);
    } 

    mem_free(filePath);
}

/**************** indexPage() ****************/
/* Scans html data, creating an inverted index linking found words to counters                     */
/* The counters has the docID for the scan page as a key and the number of occurences as the item. */
static void
indexPage(index_t* index, webpage_t* page, int docID)
{
    char* word;
    int pos = 0;
    /* steps through each word of the webpage, */
    // allocs memory for new word
    while((word = webpage_getNextWord(page, &pos)) != NULL) {
        /* skips trivial words (less than length 3), */
        if (strlen(word) < 3) {
            free(word);
            continue;
        }
        /* normalizes the word (converts to lower case), */
        // adjust word to lowercase
        char* ptr = word;
        // iterate char* pointer to lower every character
        while (*ptr) {
            *ptr = tolower(*ptr);
            ptr++;
        }
        /* looks up the word in the index */
        // word count for docID is incremented if already present
        // counters_t* is created for word key with docID if absent
        index_add(index, word, docID);
        // free word
        mem_free(word);
    }
}
