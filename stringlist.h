//
// Created by kotzaboss on 20/02/2021.
//

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
void        StringList_push_back (StringList sl, const char* str);
const char* StringList_at        (StringList sl, int idx);
int         StringList_size      (StringList sl);

//void stringlist_set_at(StringList sl, int idx, const char* val);
//char* const* stringlist_release(StringList sl);
//char* stringlist_join(StringList sl, char* outbuff, int size, const char* btw);

#endif //STRINGLIST_H
