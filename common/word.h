/* 
 * word.h    Kyrylo Bakumenko    12 May, 2023
 */

#include <stdlib.h>
#include <string.h>

/**************** functions ****************/

/**************** parse_words ****************/
/* uses strtok to tokenize a string by POSIX whitespace,
 * seperated tokens populate an array of words,
 * te number of tokens is recorded in numWords
 *
 * Caller provides:
 *   words pointer array with at least the size of line
 *   a valid char pointer "line" and a int pointer
 * We return:
 *   void
 * We guarantee:
 *   no additional memory is allocated for words
 *   original string is unchanged
 */
void parse_words(char** words, char* line, int* numWords);
