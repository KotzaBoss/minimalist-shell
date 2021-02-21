//
// Created by kotzaboss on 20/02/2021.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interpret.h"
#include "stringlist.h"


/**
 * @brief Struct containing pointer to the command string and the first argument.
 */
struct PipeSegmentMeta
{
	char* cmd;  // TODO solve name class with CMD, perhaps bin?
	StringList args;  // TODO convert to array of args ["-la", "."
};

/**
 * @brief Struct containing an array of pipe segment and size metadata.
 */
struct PipeSegments
{
	PipeSegmentMeta* metas;
	int size;
};

struct CMD {
	char** cmd;
};

PipeSegmentMeta new_pipesegmentmeta() {
	return malloc(sizeof(struct PipeSegmentMeta));
}

/**
 * @brief Tokenize individual pipe segment string.
 */
static
PipeSegmentMeta inner_tokenize(char* start)
{
	PipeSegmentMeta cmd_meta = new_pipesegmentmeta();

	StringList args = stringlist_new();
	char* token = strtok(start, CMD_TOKENS);
	cmd_meta->cmd = token;
	token = strtok(NULL, CMD_TOKENS);
	while (token) {
		stringlist_push_back(args, token);
		token = strtok(NULL, CMD_TOKENS);
	}

	cmd_meta->args = args;
	return cmd_meta;
}

/**
 * @brief Generate new PipeSegments struct from line read from stdin.
 */
PipeSegments new_pipe_segments(char* line, int max_segments)
{
	PipeSegments psegs = malloc(sizeof(struct PipeSegments));
	psegs->metas = malloc(max_segments * sizeof(struct PipeSegmentMeta));
	psegs->size = 0;

	char* segments[max_segments];  // TODO: make vector
	for (char* token = strtok(line, PIPE_TOKENS);
	     token;
	     token = strtok(NULL, PIPE_TOKENS), ++psegs->size) {
		segments[psegs->size] = token;
	}

	for (int i = 0; i < psegs->size; ++i) {  // TODO array of arguments char** ["asd", "asd]
		psegs->metas[i] = inner_tokenize(segments[i]);
	}

	return psegs;
}

int pipesegments_size(PipeSegments ps) {
	return ps->size;
}

PipeSegmentMeta* pipesegments_metas(PipeSegments ps) {
	return ps->metas;
}

CMD cmd_new(PipeSegmentMeta psm)
{
	char* cmd = psm->cmd;
	StringList args = psm->args;
	int arg_list_len = 1 + stringlist_size(args) + 1;  // cmd + args + NULL

	CMD arg_list = malloc(sizeof(CMD));
	arg_list->cmd = malloc(arg_list_len * sizeof(char*));

	arg_list->cmd[0] = cmd;
	for (int i = 1; i < arg_list_len - 1; ++i)
		arg_list->cmd[i] = stringlist_at(args, i - 1);
	arg_list->cmd[arg_list_len - 1] = NULL;  // {"cmd", "arg1", "args2", ..., NULL}

	return arg_list;
}

void cmd_free(CMD cmd)
{
	free(cmd);
}

char** cmd_release(CMD cmd) {
	return cmd->cmd;
}

void free_pipe_segments(PipeSegments ps)
{
	for (int i = 0; i < ps->size; ++i) {
		free(ps->metas[i]);
	}
	free(ps->metas);
	free(ps);
}

