/* querier.c    Kyrylo Bakumenko    6 May, 2023
 *
 * This file implements the Querier module for the TSE.
 * 
 * Exit codes: 1 -> invalid number of arguments
 *             2 -> one or more arguments are null
 *             3 -> directory path is not valid
 *             4 -> provided directory is not a crawler director
 *             5 -> the file indexFilename cannot be read
 */

#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include "index.h"
#include "word.h"
#include "file.h"
#include "mem.h"

// provided by stdio
int fileno(FILE *stream);
// internal function prototypes
static void query(index_t* index, char* pageDirectory);
static void page_rank(int* scores, int size, char* pageDirectory);
static bool parse_query(char** words, char* query, int* numWords);
static bool verify_query(char** words, int numWords);
// helper functions
static void mergeSort(int scores[], int l, int r, int idxs[]);
static void merge(int scores[], int l, int m, int r, int idxs[]);

/* ***************************
 *  main function
 *  Accepts 2 arguments: pageDirectory, indexFilename
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

    // assign names to arguments
    char* pageDirectory = argv[1];
    char* indexFilename = argv[2];
    index_t* index;

    char* filePath = mem_malloc(strlen(pageDirectory) + strlen("/.crawler") + 1);
    FILE* fp;
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

    // check if this is a crawler directory
    strcat(filePath, "/.crawler");
    if ((fp = fopen(filePath, "r")) == NULL) {
        fprintf(stderr, "ERROR: .crawler not in %s directory!\n", pageDirectory);
        mem_free(filePath);
        exit(4);
    }
    mem_free(filePath);
    fclose(fp);

    // test if indexFilename can be read
    if ((fp = fopen(indexFilename, "r")) == NULL) {
        fprintf(stderr, "ERROR: Cannot open file %s\n", indexFilename);
        exit(5);
    }

    // if succesful, determine length of file
    const int size = file_numLines(fp);
    fclose(fp);
    // initialize index to length of file (same as num of words)
    index = index_new(size);

    /* load the index from indexFilename into an internal data structure */
    index_load(index, indexFilename);

    /* read search queries from stdin, one per line, until EOF */
    query(index, pageDirectory);
    
    // memory cleanup
    index_delete(index);
    mem_free(index);

    return 0; // exit status
}

/**************** query() ****************/
/* loops through stdin query entries        */
/* evokes parse_query to parse query        */
/* evokes index_search to get page scores   */
/* evokes page_rank to rank pages by score  */
static void
query(index_t* index, char* pageDirectory)
{
    /* read search queries from stdin, one per line, until EOF */
    char* query;
    int numDocs = num_docs_crawled(pageDirectory);
    while (!feof(stdin)) {
        if (isatty(fileno(stdin))) {
            fprintf(stdout, "\nPlease enter your query: ");
        }
        query = file_readLine(stdin);
        if (query != NULL && strlen(query) != 0) {
            // there must be less words in query than characters (FACT)
            char* words[strlen(query)];
            int numWords = 0;

            if (!parse_query(words, query, &numWords)) {
                // if there is an error with the query, ignore
                mem_free(query);
                continue;
            }
            /* use the index to identify the set of documents that satisfy the query, as described below */

            // the number of docID's from the directory
            int scores[numDocs];
            // initialize to zeros
            for (int i = 0; i < numDocs; i++) {
                scores[i] = 0;
            }
            index_search(index, words, scores, numDocs, numWords);

            page_rank(scores, numDocs, pageDirectory);
            // formatting between queries
            fprintf(stdout, "-----------------------------------------------");
        } 
        mem_free(query);
    }
    // formatting: after EOF add new line
    fprintf(stdout, "\n");
}

/**************** page_rank() ****************/
/* rank pages in decreasing order of score   */
/* print score, docID, and URL for each      */
static void
page_rank(int* scores, int size, char* pageDirectory)
{
    int idxs[size];
    
    for (int i = 0; i < size; i++) {
        idxs[i] = i+1;
    }
    // sort
    mergeSort(scores, 0, size-1, idxs);

    // check if empty results
    if (scores[size-1] == 0) {
        fprintf(stdout, "\nNo documents match.\n");
        return;
    }

    // print in decreasing order
    int i = size-1;
    FILE* fp;
    int DOC_ID_MAX_LEN = 20; // ASSUMPTION: there are less than 10^19 docID's
    char docString[DOC_ID_MAX_LEN];
    char* filePath;


    // if score non-trivial (greater than 0)
    while (scores[i] > 0 && i >= 0) {
        // create filePath string
        filePath = mem_malloc(strlen(pageDirectory) + DOC_ID_MAX_LEN + 1);
        sprintf(docString, "%d", idxs[i]);
        strcpy(filePath, pageDirectory);
        strcat(filePath, "/");
        strcat(filePath, docString);
        if ((fp = fopen(filePath, "r")) == NULL) {
            fprintf(stdout, "DOC ID: %d FROM IDXS AT INDEX %d\n", idxs[i], i);
            fprintf(stderr, "ERROR: Cannot read %s\n", filePath);
            mem_free(filePath);
            i--;
            continue;
        }
        // read URL
        char* URL;
        if (!feof(fp)) {
            URL = file_readLine(fp);
            fclose(fp);
        }
        fprintf(stdout, "\nScore:\t%d\tDocID:\t%d\tURL:\t%s\n", scores[i], idxs[i], URL);
        mem_free(filePath);
        mem_free(URL);
        i--;
    }


}

/**************** mergeSort() ****************/
/* performs recursive merge sort on scores array   */
/* sort on scores is copied onto idxs array        */
static void 
mergeSort(int scores[], int l, int r, int idxs[])
{
    if (l < r) {
        int m = l + (r - l) / 2;

        mergeSort(scores, l, m, idxs);
        mergeSort(scores, m + 1, r, idxs);
 
        merge(scores, l, m, r, idxs);
    }
}

/**************** merge() ****************/
/* performs merge sort on scores array          */
/* sort on scores is copied onto idxs array     */
static void 
merge(int scores[], int l, int m, int r, int idxs[])
{
    int i, j, k;
    int len1 = m - l + 1;
    int len2 = r - m;
    // temp arrays
    int L[len1], R[len2];
    int L2[len1], R2[len2];
 
    // copy data
    for (i = 0; i < len1; i++) {
        L[i] = scores[l + i];
        L2[i] = idxs[l + i];
    }
    for (j = 0; j < len2; j++) {
        R[j] = scores[m + 1 + j];
        R2[j] = idxs[m + 1 + j];
    }
 
    // merge arrays, sorting
    i = 0; // idx of left subarray
    j = 0; // idx of right subarray
    k = l; // idx of merged array
    while (i < len1 && j < len2) {
        if (L[i] <= R[j]) {
            scores[k] = L[i];
            idxs[k] = L2[i];
            i++;
        }
        else {
            scores[k] = R[j];
            idxs[k] = R2[j];
            j++;
        }
        k++;
    }
    while (i < len1) {
        scores[k] = L[i];
        idxs[k++] = L2[i++];
    }
    while (j < len2) {
        scores[k] = R[j];
        idxs[k++] = R2[j++];
    }
}

/**************** parse_query() ****************/
/* prepares query for parse_words by converting to lowercase  */
/* evokes parse_words, prints cleaned query                   */
static bool
parse_query(char** words, char* query, int* numWords)
{
    /* translate all upper-case letters on the input line into lower-case */
    char* ptr = query;
    // iterate char* pointer to lower every character
    while (*ptr) {
        *ptr = tolower(*ptr);
        ptr++;
    }

    /* parse each query */

    // since query is non-empty, numWords >= 1
    parse_words(words, query, numWords);

    // verify query
    if (!verify_query(words, *numWords)) {
        // if query contians error, return false
        return false;
    }

    /* print the 'clean' query for user to see */
    printf("\nQuery:");
    int i = 0;
    while (words[i] != NULL) {
        printf(" %s", words[i++]);
    }

    return true;
}

/**************** verify_query() ****************/
/* verifies that query complies with requirments as outlined in README.md  */
static bool
verify_query(char** words, int numWords)
{
    // defensive programming for empty / only whitespace
    if (words == NULL || words[0] == NULL) {
        return false;
    }
    // check for bad characters
    for (int i = 0; i < numWords; i++) {
        char* word = words[i];
        size_t j;
        for (j = 0; word[j]; j++) {
            if (!isalpha(word[j])) {
                fprintf(stderr, "\nERROR: bad character '%c' in query\n", word[j]);
                return false;
            }
        }
    }
    // check for start with and/or
    if (strcmp(words[0], "and") == 0 || strcmp(words[0], "or") == 0) {
        fprintf(stderr, "ERROR: '%s' cannot be first\n", words[0]);
        return false;
    }
    // check for end with and/or
    if (strcmp(words[numWords-1], "and") == 0 || strcmp(words[numWords-1], "or") == 0) {
        fprintf(stderr, "ERROR: '%s' cannot be last\n", words[numWords-1]);
        return false;
    }
    // check for adjacent and/or
    for (int i = 1; i < numWords; i++) {
        if ( (strcmp(words[i], "and") == 0 || strcmp(words[i], "or") == 0)
        && (strcmp(words[i-1], "and") == 0 || strcmp(words[i-1], "or") == 0)) {
            fprintf(stderr, "ERROR: '%s' and '%s' cannot be adjacent\n", words[i-1], words[i]);
            return false;
        }
    }
    return true;
}
