/* 
 * pagedir.h    Kyrylo Bakumenko    23 April, 2023
 */

#ifndef __PAGEDIR_H
#define __PAGEDIR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../libcs50/webpage.h"
#include "../libcs50/mem.h"
/**************** functions ****************/

/**************** pagedir_init ****************/
/* Creates a .crawler file in specified directory
 *
 * Caller provides:
 *   a valid path to an existing directory.
 * We return:
 *   bool, true if creation was succesful and false
 *   if any errors were encountered.
 */
bool pagedir_init(const char* pageDirectory);

/**************** pagedir_save ****************/
/* Saves URL from a webpage as a file in a specified
 * directory labeled with the docID parameter
 *
 * Caller provides:
 *   a valid webpage pointer, valid path to an
 *   existing directory, and a document ID as an int.
 * We return:
 *   URL, Depth, and HTML content of the webpage is
 *   printed to terminal
 */
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);

#endif // __PAGEDIR_H