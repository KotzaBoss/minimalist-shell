//
// Created by kotzaboss on 20/02/2021.
//

#ifndef STRINGLIST_H
#define STRINGLIST_H

#include <stdbool.h>


typedef struct StringList* StringList;

StringList stringlist_new();
void stringlist_reserve(StringList sl, int newalloc);
void stringlist_push_back(StringList sl, const char* str);
void stinglist_delete(StringList sl);
const char* stringlist_at(StringList sl, int idx);
void stringlist_set_at(StringList sl, int idx, const char* val);
const char** stringlist_release(StringList sl);
char* stringlist_join(StringList sl, char* outbuff, int size, const char* btw);
int stringlist_size(StringList sl);

#endif //STRINGLIST_H
