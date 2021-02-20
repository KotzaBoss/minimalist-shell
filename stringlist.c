//
// Created by kotzaboss on 20/02/2021.
//

#include "stringlist.h"
#include <stdlib.h>
#include <string.h>

#define INIT_SPACE 10

struct StringList {
	const char** array;
	int space;
	int size;
};

StringList stringlist_new()
{
	StringList sl = malloc(sizeof(struct StringList));
	if (sl) {
		sl->size = 0;
		sl->space = 0;
		sl->array = NULL;
	}
	return sl;
}

void stinglist_delete(StringList sl) {
	free(sl->array);
	free(sl);
}

void stringlist_reserve(StringList sl, int newalloc) {
	if(newalloc <= sl->size)
		return;
	const char** tmp = sl->array;
	sl->array = malloc(newalloc * sizeof(char*));
	for(int i = 0; i < sl->size; ++i)
		sl->array[i] = tmp[i];
	free(tmp);
	sl->space = newalloc;
}

void stringlist_push_back(StringList sl, const char* str) {
	if(!sl->array)
		stringlist_reserve(sl, INIT_SPACE);
	else if (sl->size == sl->space)
		stringlist_reserve(sl, 2 * sl->space);
	sl->array[sl->size++] = str;
}

const char* stringlist_at(StringList sl, int idx) {
	if(idx > sl->size || (!sl->array))
		return NULL;
	return sl->array[idx];
}

void stringlist_set_at(StringList sl, int idx, const char* val) {
	if(idx > sl->size || (!sl->array))
		return;
	sl->array[idx] = val;
}

int stringlist_size(StringList sl) {
	return sl->size;
}

const char** stringlist_release(StringList sl) {
	return sl->array;
}

char* stringlist_join(StringList sl, char* outbuff, int buffsize, const char* btw) {
	unsigned long total_len = 0;
	for(int i = 0; i < sl->size; ++i) {
		total_len += strlen(sl->array[i]);
	}
	total_len += (sl->size - 1) * strlen(btw);

	int buff_idx = 0;
	for(int sl_idx = 0; sl_idx < sl->size; ++sl_idx) { // Last element doesnt have btw after it
		for(int elem_idx = 0; buff_idx < buffsize && elem_idx < strlen(sl->array[sl_idx]); ++elem_idx, ++buff_idx) {
			outbuff[buff_idx] = sl->array[sl_idx][elem_idx];
		}
		if (sl_idx == sl->size - 1)
			break;
		for(int btw_idx = 0; buff_idx < buffsize && btw_idx < strlen(btw); ++btw_idx, ++buff_idx) {
			outbuff[buff_idx] = btw[btw_idx];
		}
	}
	return outbuff;
}