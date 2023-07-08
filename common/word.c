/* word.c - CS50 'common' module
 *
 * Module providing certain helper function for the querier module in CS50 TSE
 * 
 * Kyrylo Bakumenko, 12 May 2023
 */

#include <stdlib.h>
#include <string.h>

/**************** global functions ****************/
/* that is, visible outside this file                        */
void parse_words(char** words, char* line, int* numWords);

/**************** parse_words() ****************/
/* uses strtok to tokenize a string by POSIX whitespace */
void
parse_words(char** words, char* line, int* numWords)
{
    char delimit[]=" \t\r\n\v\f"; // POSIX whitespace characters (isspace())
    int i = 0;

    words[i] = strtok(line, delimit);
    while (words[i]!=NULL) {
        (*numWords)++;
        words[++i] = strtok(NULL, delimit);
    }
}

