#include <stdlib.h>
#include <string.h>
#include "stringlist.h"

#define INIT_SPACE 10

struct StringList
{
	const char** array;
	int space;
	int size;
};

StringList StringList_new()
{
	StringList sl = malloc(sizeof(struct StringList));
	if (sl) {
		sl->size = 0;
		sl->space = 0;
		sl->array = NULL;
	}
	return sl;
}

void StringList_free(StringList* sl)
{
	free((*sl)->array);
	free(*sl);
	*sl = NULL;
}

void StringList_reserve(StringList sl, int newalloc)
{
	if (newalloc <= sl->size)
		return;
	const char** tmp = sl->array;
	sl->array = malloc(newalloc * sizeof(char*));
	for (int i = 0; i < sl->size; ++i)
		sl->array[i] = tmp[i];
	free(tmp);
	sl->space = newalloc;
}

void StringList_push_back(StringList sl, const char* str)
{
	if (!sl->array)
		StringList_reserve(sl, INIT_SPACE);
	else if (sl->size == sl->space)
		StringList_reserve(sl, 2 * sl->space);
	sl->array[sl->size++] = str;
}

const char* StringList_at(StringList sl, int idx)
{
	if (idx > sl->size || (!sl->array))
		return NULL;
	return sl->array[idx];
}

int StringList_size(StringList sl)
{
	return sl->size;
}
