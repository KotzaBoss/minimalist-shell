#include <wordexp.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
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
	bool wait;
};

struct CMD
{
	const char** cmd;
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

/**
 * @brief Overwrite buffer adding escape backslash for the esc_chr characters
 */
static
void escape(char* buffer, int max_size, const char* esc_chr)
{
	char* tmp = calloc(sizeof(char), max_size);
	char* err_pos[100] = {NULL};
	char* err = buffer;
	for (int i = 0; i < 100 && (err = strstr(err, "$?")); err_pos[i++] = err) {
		err[0] = err[1] = ' ';
	}

	for (int i = 0, buff_i = 0; i < max_size; ++i, ++buff_i) {
		for (int j = 0; j < strlen(esc_chr); ++j) {
			if (esc_chr[j] == buffer[buff_i]) {
				tmp[i++] = '\\';
			}
			tmp[i] = buffer[buff_i];
		}
		for (int e = 0; e < 100; ++e) {
			if (&buffer[buff_i] == err_pos[e]) {
				i += sprintf(&tmp[i], "%d", errno);
				break;
			}
		}
		tmp[i] = buffer[buff_i];
	}
	for (int i = 0; i < max_size; ++i) {
		buffer[i] = tmp[i];
	}
	free(tmp);
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
	psegs->wait = true;

	if (line[strlen(line) - 1] == '&') {
		psegs->wait = false;
	}
	char* wait_chr = strchr(line, WAIT_CHAR);
	if (wait_chr) {
//		if ((wait_chr[1] != '\0') && (wait_chr[1] != '\n')) {  // If not at the end
//			PipeSegments_free(&psegs);
//			return NULL;
//		}
//		else {
		*wait_chr = '\0';
//		}
	}

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

PipeSegmentMeta* PipeSegments_metas(PipeSegments ps)
{ return ps->metas; }

int PipeSegments_size(PipeSegments ps)
{ return ps->size; }

PipeSegmentMeta PipeSegments_last(PipeSegments ps)
{ return ps->size > 0 ? ps->metas[ps->size - 1] : NULL; }

bool PipeSegments_wait(PipeSegments ps)
{ return ps->wait; }

CMD CMD_new(PipeSegmentMeta psm)
{
	int total_arg_len = 1 + StringList_size(psm->args) + 1;  // cmd + args + NULL

	CMD cmd = malloc(sizeof(struct CMD));
	cmd->cmd = malloc(total_arg_len * sizeof(char*));

	cmd->cmd[0] = psm->cmd;
	for (int i = 1; i < total_arg_len - 1; ++i)
		cmd->cmd[i] = StringList_at(psm->args, i - 1);
	cmd->cmd[total_arg_len - 1] = NULL;  // {"cmd", "arg1", "args2", ..., NULL}

	return cmd;
}

void CMD_free(CMD* cmd)
{
	free((*cmd)->cmd);
	free(*cmd);
	*cmd = NULL;
}

const char** CMD_release(CMD cmd)
{ return cmd->cmd; }

const char* CMD_name(CMD cmd)
{ return cmd->cmd[0]; }

char* expand_buffer(char* buffer, int max_size)
{
//	buffer[strcspn(buffer, "\n")] = '\0';  // wordexp gets confused with "\n"
	escape(buffer, max_size, "|&");

	wordexp_t w;
	wordexp(buffer, &w, 0);
	size_t expanded_length = 0;
	for (int i = 0; i < w.we_wordc; ++i) {
		expanded_length += strlen(w.we_wordv[i]) + 1;  // + space
	}
	++expanded_length; // NULL termination
	if (expanded_length > max_size) {
		return NULL;
	}

	int buff_i = 0;
	for (int w_str = 0; w_str < w.we_wordc; ++w_str) {
		for (int w_str_i = 0; w_str_i < strlen(w.we_wordv[w_str]); ++w_str_i, ++buff_i) {
			buffer[buff_i] = w.we_wordv[w_str][w_str_i];
		}
		buffer[buff_i++] = ' ';
	}
	buffer[--buff_i] = '\0'; // undo last space
	wordfree(&w);
	return buffer;
}