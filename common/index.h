/* 
 * index.h    Kyrylo Bakumenko    23 April, 2023
 */

#include <stdlib.h>
#include "hashtable.h"
#include "counters.h"
#include "mem.h"

/**************** global types ****************/
typedef struct index index_t;

/**************** functions ****************/

/**************** index_new ****************/
/* Create a new (empty) index.
 *
 * Caller provides:
 *   number of slots to be used for the index (must be > 0).
 * We return:
 *   pointer to a new index; return NULL if error.
 * We guarantee:
 *   index is initialized empty.
 * Caller is responsible for:
 *   later calling index_delete.
 */
index_t* index_new(const int size);

/**************** index_save ****************/
/* Saves the index to a file specified by indexFilename
 *
 * Caller provides:
 *   a valid path to an existing directory and filename
 *   If the file does not yet exist, it will be created
 * We return:
 *   void
 */
void index_save(index_t* index, char* indexFilename);

/**************** index_save ****************/
/* Saves the index to a file specified by indexFilename
 *
 * Caller provides:
 *   a valid path to an existing directory and filename
 *   If the file does not yet exist, it will be created
 * We return:
 *   void
 */
void index_load(index_t* index, char* indexFilename);

/**************** index_delete ****************/
/* Frees memory associated within index struture
 *
 * Caller provides:
 *   valid pointer to an index
 * We return:
 *   void
 */
void index_delete(index_t* index);

/**************** index_find ****************/
/* Return the item associated with the given key.
 *
 * Caller provides:
 *   valid pointer to index, valid string for key.
 * We return:
 *   pointer to the item corresponding to the given key, if found;
 *   NULL if index is NULL, key is NULL, or key is not found.
 * Notes:
 *   the index is unchanged by this operation.
 */
counters_t* index_find(index_t* index, char* key);

/**************** index_add ****************/
/* Increments the count associated with the docID
 * as accessed by the key from index
 *
 * Caller provides:
 *   valid pointer to index, valid string for key, and a docID.
 * We return:
 *   true if docID's count was succesfully updated in index
 *   false otherwise
 */
bool index_add(index_t* index, char* key, int docID);

/**************** index_search ****************/
/* initializes score array with score, representing
 * relevance of a word as defined in specs. This is done
 * for every word in words which return a non-null counter
 * with a look-up in the index.
 *
 * Caller provides:
 *   valid index pointer, array of char*, int array of scores
 *   as well as size variables numDocs and numWords
 * We return:
 *   void
 * We guarantee:
 *   index is unchanged
 */
void index_search(index_t* index, char** words, int* scores, int numDocs, int numWords);

/**************** num_docs_crawled ****************/
/* Returns the number of crawled docs in a crawler directory
 * Assumes directory *is* a crawler directory without extensive checks
 *
 * Caller provides:
 *   A valid path to an existing readable crawler directory
 *   Directory must have already been crawled (populated with docs)
 * We return:
 *   0 if directory does not follow crawler directory format
 *   Otherwise, return last docID not returning null (# of docs)
 */
int num_docs_crawled(char* pageDirectory);
