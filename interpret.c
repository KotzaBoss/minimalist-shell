//
// Created by kotzaboss on 20/02/2021.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interpret.h"
#include "stringlist.h"


struct PipeSegmentMeta
{
	char* cmd;  // TODO solve name class with CMD, perhaps bin?
	StringList args;
};

struct PipeSegments
{
	PipeSegmentMeta* metas;
	int size;
};

struct CMD
{
	char** cmd;
};

/**
 * @brief Tokenize individual pipe segment string producing a PipeSegmentMeta.
 */
static
PipeSegmentMeta inner_tokenize(char* start)
{
	PipeSegmentMeta cmd_meta = PipeSegmentMeta_new();

	StringList args = StringList_new();
	char* token = strtok(start, CMD_TOKENS);
	cmd_meta->cmd = token;
	token = strtok(NULL, CMD_TOKENS);
	while (token) {
		StringList_push_back(args, token);
		token = strtok(NULL, CMD_TOKENS);
	}

	cmd_meta->args = args;
	return cmd_meta;
}

PipeSegmentMeta PipeSegmentMeta_new()
{
	return malloc(sizeof(struct PipeSegmentMeta));
}

void PipeSegmentMeta_free(PipeSegmentMeta* psm)
{
	StringList_free(&(*psm)->args);
	free(*psm);
	*psm = NULL;
}

PipeSegments PipeSegments_new(char* line, int max_segments)
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

void PipeSegments_free(PipeSegments* ps)
{
	for (int i = 0; i < (*ps)->size; ++i) {
		if ((*ps)->metas[i])
			PipeSegmentMeta_free(&(*ps)->metas[i]);
	}
	free((*ps)->metas);
	free(*ps);
	*ps = NULL;

}

PipeSegmentMeta* PipeSegments_metas(PipeSegments ps) { return ps->metas; }
int PipeSegments_size(PipeSegments ps) { return ps->size; }

CMD CMD_new(PipeSegmentMeta psm)
{
	char* cmd = psm->cmd;
	StringList args = psm->args;
	int arg_list_len = 1 + StringList_size(args) + 1;  // cmd + args + NULL

	CMD arg_list = malloc(sizeof(struct CMD));
	arg_list->cmd = malloc(arg_list_len * sizeof(char*));

	arg_list->cmd[0] = cmd;
	for (int i = 1; i < arg_list_len - 1; ++i)
		arg_list->cmd[i] = StringList_at(args, i - 1);
	arg_list->cmd[arg_list_len - 1] = NULL;  // {"cmd", "arg1", "args2", ..., NULL}

	return arg_list;
}

void CMD_free(CMD* cmd)
{
	free((*cmd)->cmd);
	free(*cmd);
	*cmd = NULL;
}

char** CMD_release(CMD cmd) { return cmd->cmd; }

const char* CMD_name(CMD cmd) { return cmd->cmd[0];}
