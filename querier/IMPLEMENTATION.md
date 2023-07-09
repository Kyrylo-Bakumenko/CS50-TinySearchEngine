# CS50 TSE Querier
## Implementation Spec

In this document we reference the [Requirements Specification](REQUIREMENTS.md) and [Design Specification](DESIGN.md) and focus on the implementation-specific decisions.
Here we focus on the core subset:

-  Data structures
-  Control flow: pseudo code for overall flow, and for each of the functions
-  Detailed function prototypes and their parameters
-  Error handling and recovery
-  Testing plan

## Data structures 

No new data strctures are introduced in this module. However, we make use of the *index* data structure to load data with `index_load`. More information can be found in the *common* module in `index.h`. The index is intialized with the size of the directory at the `pageDirectory` path, that is the number of docs following the naming scheme from the *crawler* module.

## Control flow

Querier is implemented in one file `querier.c`, with four static functions for querying (`query`, `page_rank`, `parse_query`, and `verify_query`) as well as two static sort-helper functions (`mergeSort` and `merge`). `qurier.c` also makes use of four functions (`index_new`, `index_load`, `index_delete`, and `num_docs_crawled`) implemented in `index.c` and one function (`parse_words`) implemented in `word.c`. There is a second file, `fuzzquery.c` which aids with the tesing of the Querier module by providing sample query input. `fuzzquery.c` is utilized in `testing.sh` but not by `querier` directly.

### main

`querier.c` has the `main` function call `index_new`, `index_load`, `query`, `index_delete` and then exits zero.

### query

loops through query input from stdin. Processes, parses, indexes, and outputs ranking resulting from query input matching with the docs form the specifid crawled directory.

Pseudocode:
	read search queries from stdin, one per line, until EOF:
		read a line from stdin as query
		print formatting for query if tty 
		parse the input query with parse_query 
		if valid:
			search index for occured words in query with index_search
			rank resulting docs with page_rank and output results

### parse_query

Normalizes input by transfering to lowercase, breaks query into an array of strings

Pseudocode:
	for every character in query:
		replace with lowercase equivalent
	tokenize query with parse_words, store in a char** words
	verify words cotains only legal query input with verify_query
	print the'cleaned' query

### parse_words

Tokenizes an input string by splitting by POSIX whitespace, reulting stirngs populate the char** words

Pseudocode:
	define delimitier as POSIX whitespace
	while strtok odf input is not null:
		insert into words array

### verify_query

Verifies that query uses only allowed syntax as outlined in the `REQUIRMENTS.md`

Pseudocode:
	verify that words is not null and contains at least one word
	for every cahracter verify that it contains no bad characters
	verify that and/or do not begin a query
	verify that and/or do not end a query
	for every word in words, loop and:
		verify that and/or are not adjacent

## Other modules

### index_new, index_load, index_delete, and num_docs_crawled

These functions are imported from their implementation in `index.c`. See the *indexer* module's `IMPLEMENTATION.md` and *common*'s `index.h`for more infromation on these functions.

### fileno

The function `fileno` is implemented by the *stdio* standard C library, however, it is not therein declared. This function is used and declared in `querier.c`.

## Function prototypes

### querier

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's implementation in `querier.c` and is not repeated here.

```c
static void query(index_t* index, char* pageDirectory);
static void page_rank(int* scores, int size, char* pageDirectory);
static bool parse_query(char** words, char* query, int* numWords);
static bool verify_query(char** words, int numWords);
static void mergeSort(int scores[], int l, int r, int idxs[]);
static void merge(int scores[], int l, int m, int r, int idxs[]);
```

## Error handling and recovery

All the command-line parameters are rigorously checked before any data structures are allocated or work begins; problems result in a message printed to stderr and a non-zero exit status. Exit statuses are described in header block comments at the start of the files.

We anticipate out-of-memory errors to be rare and thus allow the program to crash (cleanly) in this way.

## Testing plan

Here is an implementation-specific testing plan.

### Integration/system testing

We write a script `testing.sh` that invokes querier several times, with a variety of command-line arguments.
The main unit is `testing.sh` and the required files `test1`, `test1_correct.out`, `test2_correct.out`, `test3_correct.out`.

First, a sequence of invocations with erroneous arguments, read/write permissions, and extraneus non-crawler directories, each testing the possible mistakes that can be made.
Second, multiple iterations over two crawler directories: `../data/letters-10` and `../data/toscrape-2`, which are obtained by running crawler on letters and toscrape seed URL's at depths 10 and 2 respectively. Additionally, `testing.sh` expects the files  `../data/letters-10/index.nd` and `../data/toscrape-2/index.ndx` to exist, obtained by running `indexer.c` on the aforementioned directories and filenames respectively.
Third, a run with valgrind to verify there are no memory leaks.
The script is evoked with `bash -v testing.sh` so the output of crawler is intermixed with the commands used to invoke the crawler.
Verify correct behavior by studying the output, and by sampling the files created in the respective pageDirectories.

The output of `testing.sh` is internally created temporary files `test1.out`, `test2.out`, `test3.out`. These output files are compared with the aforementioned `test*_correct.out` files which have been manually erified to contain correct output.

The `querier.c` module is tested by observing its behavior during the system test.
