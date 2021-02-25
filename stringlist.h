#ifndef STRINGLIST_H
#define STRINGLIST_H

#include <stdbool.h>

/**
 * @brief Expandable array of strings. (Vector<String>)
 */
typedef struct StringList* StringList;

StringList  StringList_new       ();
void        StringList_free      (StringList* sl);

/**
 * @brief Reserve `newalloc` space beyond current space. Useful to avoid
 * multiple re-allocations if size required is known
 */
void        StringList_reserve   (StringList sl, int newalloc);

/**
 * @brief Append to the array. Should there not be enough space double size.
 */
void        StringList_push_back (StringList sl, const char* str);

/**
 * @brief Return element at index idx
 */
const char* StringList_at        (StringList sl, int idx);

/**
 * @brief Return size of string list
 */
int         StringList_size      (StringList sl);

#endif //STRINGLIST_H
