//
// Created by kotzaboss on 20/02/2021.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interpret.h"
#include "stringlist.h"


/**
 * @brief Tokenize individual pipe segment string.
 */
PipeSegmentMeta inner_tokenize(char* start)
{
	PipeSegmentMeta cmd_meta;

	StringList args = stringlist_new();
	char * token = strtok(start, CMD_TOKENS);
	cmd_meta.cmd = token;
	token = strtok(NULL, CMD_TOKENS);
	while(token) {
		stringlist_push_back(args, token);
		token = strtok(NULL, CMD_TOKENS);
	}

	cmd_meta.args = args;
	return cmd_meta;
}

/**
 * @brief Generate new PipeSegments struct from line read from stdin.
 */
PipeSegments* new_pipe_segments(char* line, int max_segments)
{
	PipeSegments* psegs = malloc(sizeof(PipeSegments));
	psegs->metas = malloc(max_segments * sizeof(PipeSegmentMeta));
	psegs->size = 0;

	char* segments[max_segments];  // TODO: make vector
	for (char* token = strtok(line, PIPE_TOKENS);
	     token;
	     token = strtok(NULL, PIPE_TOKENS), ++psegs->size)
	{
		segments[psegs->size] = token;
	}

	for (int i = 0; i < psegs->size; ++i) {  // TODO array of arguments char** ["asd", "asd]
		psegs->metas[i] = inner_tokenize(segments[i]);
	}

	return psegs;
}

void free_pipe_segments(struct PipeSegments* ps)
{
	free(ps->metas);
	free(ps);
}
