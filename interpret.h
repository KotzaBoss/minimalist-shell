//
// Created by kotzaboss on 20/02/2021.
//

#ifndef INTERPRET_H
#define INTERPRET_H

#include "stringlist.h"

#define PIPE_TOKENS "|\n"
#define CMD_TOKENS " \n"


/**
 * @brief Struct containing pointer to the command string and the first argument.
 */
typedef struct PipeSegmentMeta
{
	char* cmd;
	StringList args;  // TODO convert to array of args ["-la", "."
} PipeSegmentMeta;

/**
 * @brief Struct containing an array of pipe segment and size metadata.
 */
typedef struct PipeSegments
{
	PipeSegmentMeta* metas;
	int size;
} PipeSegments;

PipeSegmentMeta inner_tokenize(char* start);
PipeSegments* new_pipe_segments(char* line, int max_segments);
void free_pipe_segments(PipeSegments* ps);
#endif //INTERPRET_H
