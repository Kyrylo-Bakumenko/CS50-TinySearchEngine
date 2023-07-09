# CS50 TSE Indexer
## Implementation Spec

In this document we reference the [Requirements Specification](REQUIREMENTS.md) and [Design Specification](DESIGN.md) and focus on the implementation-specific decisions.
Here we focus on the core subset:

-  Data structures
-  Control flow: pseudo code for overall flow, and for each of the functions
-  Detailed function prototypes and their parameters
-  Error handling and recovery
-  Testing plan

## Data structures 

We use the *index* data structure: a wrapper on a *hashtable* mapping `char* word` keys to *counter* items. This data structure is implemented in `index.c`. The counter item inside the hashtable maps an `int docID` to a `int count` where the _docID_ represents the name of a crawled file read by `indexer.c` and _count_ represents the number of times the key _word_ was found in the file _docID_. 

When creating the hashtable from the output of crawler, the size of the hashtable (slots) is impossible to determine in advance, so we use 200.

## Control flow

Indexer is implemented in one file `indexer.c`, with two static functions (`indexBuild`, `indexPage`). `indexer.c` also makes use of four functions (`index_new`, `index_save`, `index_delete`, `index_add`) implemented in `index.c`. There is a second file, `indextester.c` which aids with the tesing of the *index* struct. `indextest.c` only contains the `main` method and uses `index_new`, `index_save`, `index_load`, `index_delete` from `index.c`.

### main

`indexer.c` has the `main` function call `index_new`, `indexBuild`, `index_save`, `index_delete` and then exits zero.
`indextest.c` has the `main` function call `index_new`, `index_load`, `index_save`, `index_delete` and then exits zero.

### indexBuild

Recieves an *index_t* struct and the first argument passed from commandline (`char* pageDirectory`) as a parameters. The `index_t* index` is initialized in main, by `index_new`.

* for `pageDirectory`, assert that it is a valid path to a Crawler directory
* for every file with crawler output file in `pageDirectory`, call `indexPage`

Pseudocode:
	assert that pageDirectory is a valid path to a Crawler directory
	for every output file from that crawled dircetory:
		extract url, depth, and html data
		create a webpage_t from this infromation and pass into indexPage

### indexPage

Scans html data from a *webpage_t*, creating an inverted index linking found `char* word`'s to *counter_t* structs.
The *counter_t* has the docID for the scan page as a key and the number of occurences as the item.

Pseudocode:
	for every word in the html data:
		if the length of the word is less than three, ignore it
		else, normlaize the word to all lowercase characters
		pass into index_add

## Other modules

### pagedir

We leverage the `index` module of common.

Pseudocode for `index_new`:

	allocates memory for index
	allocates memory for hashtable in index
	return pointer to empty index

Pseudocode for `index_save`:

	verifies indexFilename can be written to
	if index is not null
		if index's hashtable is not null
			write hashtable's word key on a new line
				on each line print docID and count for every docID
				seperate by spaces

Pseudocode for `index_load`:

	if indexFilename can be written to
		for every line in the file indexFilename
			the first token seperated by spaces is a 'word' key for the index
			create a counter_t*
				save every next to tokens as int's, docID and count
				set the docID key in the counter_t* to have item count
			insert counter_t* into index

Pseudocode for `index_add`:

	for a given index, key, and docID...
	if the key exists in index
		increment the count associated with docID
	else
		create a counter_t
		set the count of docID to 1
		insert key into index with the counter_t

Pseudocode for `index_delete`:

	iteratively call hashtale_delete
		on every occurence in the hashtable, call counters delete 

### libcs50

We leverage the modules of libcs50, most notably `hashtable`, `counters`, and `webpage`.
See that directory for module interfaces.

## Function prototypes

### indexer

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's implementation in `indexer.c` and is not repeated here.

```c
static void indexBuild(index_t* index, char* pageDirectory);
static void indexPage(index_t* index, webpage_t* page, int docID);
```

### indextest

Contains no newly declared functions.

## Error handling and recovery

All the command-line parameters are rigorously checked before any data structures are allocated or work begins; problems result in a message printed to stderr and a non-zero exit status. Exit statuses are described in header block comments at the start of the files.

We anticipate out-of-memory errors to be rare and thus allow the program to crash (cleanly) in this way.

## Testing plan

Here is an implementation-specific testing plan.

### Unit testing

The main unit is indexer.
The output of indexer is tested with indextest, which attempt to recreate the file created by indexer.
The `index.c` module is tested by observing its behavior during the system test.

### Integration/system testing

We write a script `testing.sh` that invokes indexer and indextest several times, with a variety of command-line arguments.
First, a sequence of invocations with erroneous arguments, read/write permissions, and extraneus non-crawler directories, each testing the possible mistakes that can be made.
Second, a run with valgrind to verify there are no memory leaks.
The script is evoked with `bash -v testing.sh` so the output of crawler is intermixed with the commands used to invoke the crawler.
Verify correct behavior by studying the output, and by sampling the files created in the respective pageDirectories.
