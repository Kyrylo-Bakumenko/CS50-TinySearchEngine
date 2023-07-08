/* index.c - CS50 'index' module
 *
 * Module providing the data structure to represent the in-memory index, and functions to read and write index files.
 * index module for TSE
 * 
 * Kyrylo Bakumenko, 29 April 2023
 */

#include <stdlib.h>
#include <string.h>
#include "webpage.h"
#include "hashtable.h"
#include "counters.h"
#include "file.h"
#include "mem.h"

/**************** global types ****************/

typedef struct index {
    // given a word, can determine number of occurences
    // in a particular file (indexed by docID)
    //
    // char* word -> (int docID, int count)
    hashtable_t* table;
} index_t;


/**************** global functions ****************/
/* that is, visible outside this file                                */
/* see hashtable.h for comments about exported hashtable_t functions */
/* see counters.h for comments about exported counters_t functions   */

index_t* index_new(const int size);
void index_save(index_t* index, char* indexFilename);
void index_load(index_t* index, char* indexFilename);
void index_delete(index_t* index);
void index_search(index_t* index, char** words, int* scores, int numDocs, int numWords);

counters_t* index_find(index_t* index, char* key);
bool index_add(index_t* index, char* key, int docID);
int num_docs_crawled(char* pageDirectory);

static void index_itr(void* fp, const char* key, void* item);
static void index_itr_helper(void* fp, const int key, const int count);
static void index_delete_helper(void* item);


/**************** local functions ****************/
/* not visible outside this file */

/**************** index_new() ****************/
/* initialises index object                      */
/* description in index.h                        */
index_t* 
index_new(const int size)
{   
    if (size <= 0) {
        return NULL;
    }
    // allocate memory for array of pointers
    index_t* index = mem_malloc(sizeof(index_t));
    if (index == NULL) {
        return NULL;
    } else {
        // allocs mem
        index->table = hashtable_new(size);
    }
    
    return index;
}

/**************** index_save() ****************/
/* writes inverted index to specified path    */
/* description in index.h                     */
void
index_save(index_t* index, char* indexFilename)
{   
    FILE* fp;
    // indexFilename is a complete filepath
    if ((fp = fopen(indexFilename, "w")) == NULL) {
        fprintf(stderr, "ERROR: Cannot write to %s\n", indexFilename);
        exit(1);
    }

    // defensive programming
    if (index != NULL && index->table != NULL) {
        // iterate through every word in index
        hashtable_iterate(index->table, fp, index_itr);
    }

    fclose(fp);
}

/**************** index_load() ****************/
/* reads inverted index from specified path   */
/* description in index.h                     */
void
index_load(index_t* index, char* indexFilename)
{
    FILE* fp;
    /* creates index from oldIndexFilename */
    // try to open file
    if ((fp = fopen(indexFilename, "r")) != NULL) {
        // loop through every line (one word per line)
        char* line;
        while ((line = file_readLine(fp)) != NULL) {
            // save the word from the line
            char* word = strtok(line, " ");
            // assign values to docID and count pairs
            int docID;
            int count;

            // gather new docID, count pair
            // and assert that values could be read
            char* nextDocID;
            char* nextCount;
            // create counter for word
            counters_t* counter = counters_new();
            while ((nextDocID = strtok(NULL, " ")) != NULL && (nextCount = strtok(NULL, " ")) != NULL) {
                // assign value to docID and count variables
                sscanf(nextDocID, "%d", &docID);
                sscanf(nextCount, "%d", &count);

                // add docID, counter pair to counter
                counters_set(counter, docID, count);
            }
            // add word->counter to hashtable
            hashtable_insert(index->table, word, counter);

            // clear mem for line
            mem_free(line);
        }
    } else {
        // if fails, exit with error
        fprintf(stderr, "ERROR: Unable to open file: %s\n", indexFilename);
        exit(3);
    }

    fclose(fp);
}

/**************** index_delete() ****************/
/* Frees memory associated within index struture */
/* description in index.h                        */
void 
index_delete(index_t* index)
{   
    // iterate through hashtbable, calling counter_delete
    hashtable_delete(index->table, index_delete_helper);
    // free hashtable pointer [DONT DO THIS, results in double free]
    // mem_free(index->table);
}

static void
index_itr(void* fp, const char* key, void* item)
{
    // for a given entry, add word to index
    fprintf(fp, "%s", key);
    // iterate through every docID, count pair
    counters_iterate(item, fp, index_itr_helper);
    // new line for every entry
    fprintf(fp, "\n");
}

static void
index_itr_helper(void* fp, const int key, const int count)
{
    // add [docID, count] pair to index
    fprintf(fp, " %d %d", key, count);
}

static void
index_delete_helper(void* item)
{
    counters_delete(item);
}

/**************** index_find() ****************/
/* return counters_t associated with key      */
/* description in index.h                     */
counters_t* 
index_find(index_t* index, char* key)
{
    return hashtable_find(index->table, key);
}

/**************** index_add() ****************/
/* increment docID word count in index       */
/* description in index.h                    */
bool 
index_add(index_t* index, char* key, int docID)
{
    counters_t* counter;
    if (hashtable_find(index->table, key) == NULL) {
        counter = counters_new();
        counters_add(counter, docID);
        return hashtable_insert(index->table, key, counter);
    }

    counter = hashtable_find(index->table, key);
    return counters_add(counter, docID);
}

/**************** index_search() ****************/
/* initializes score array with score (relevance of word) */
/* for every word in words and with an entry in index     */
void
index_search(index_t* index, char** words, int* scores, int numDocs, int numWords) {
    int i = 0; // words index
    counters_t* counter;

    // scores is score array
    // idx 0 -> docID 1
    // the value at idx 0 is docID's score, accordingly
    
    // for every doc, (curDoc is document index)
    for (int curDoc = 0; curDoc < numDocs; curDoc++) {
        // iterate through query
        bool init = false;
        int minScore = 0;
        i = 0;
        while (i < numWords) {
            // if "and," dont bother to look up
            if (strcmp(words[i], "and") == 0) {
                i++;
                continue;
            }
            // if "or," add minScore to doc's score
            if (strcmp(words[i], "or") == 0) {
                i++;
                scores[curDoc] += minScore;
                minScore = 0;
                init = false;
                continue;
            }
            // else, try to find word
            if ((counter = index_find(index, words[i])) == NULL) {
                // if not found, minScore is 0, skip until "or"
                minScore = 0;
                while (words[i] != NULL && strcmp(words[i], "or") != 0) {
                    i++;
                }
            } else {
                // if found, running score is set to mininum query score
                int curScore = counters_get(counter, curDoc+1);
                // if uninitialized, set to curScore
                if (!init) {
                    minScore = curScore;
                    init = true;
                } else if (curScore < minScore) {
                    // else, if curScore is less than min, update
                    minScore = curScore;
                }
                i++;
            }
        }
        // terminated word sequence, last update
        scores[curDoc] += minScore;
        minScore = 0;
    }
}

/**************** num_docs_crawled() ****************/
/* Returns the number of crawled docs in a crawler directory            */
/* Assumes directory *is* a crawler directory without extensive checks  */
int
num_docs_crawled(char* pageDirectory)
{
    FILE* fp;
    int DOC_ID_MAX_LEN = 20; // ASSUMPTION: there are less than 10^19 docID's
    char docString[DOC_ID_MAX_LEN];
    char* filePath;
    int docID = 1;

    // create filePath string
    filePath = mem_malloc(strlen(pageDirectory) + DOC_ID_MAX_LEN + 1);
    sprintf(docString, "%d", docID);
    strcpy(filePath, pageDirectory);
    strcat(filePath, "/");
    strcat(filePath, docString);

    while ((fp = fopen(filePath, "r")) != NULL) {
        mem_free(filePath);
        docID++;
        filePath = mem_malloc(strlen(pageDirectory) + DOC_ID_MAX_LEN + 1);
        sprintf(docString, "%d", docID);
        strcpy(filePath, pageDirectory);
        strcat(filePath, "/");
        strcat(filePath, docString);
        fclose(fp);
    }
    mem_free(filePath);

    return docID - 1;
}
