#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>
#include "builtins.h"

#define PROMPT ">>> "
#define BUFFER_SIZE 64
#define PIPE_TOKEN "|\n"
#define TOKENS " \n"

char inbuffer[BUFFER_SIZE];

char* input(const char* prompt)
{
	printf("%s", prompt);
	return fgets(inbuffer, BUFFER_SIZE, stdin);
}

int run(int (* cmd)(const char*), const char* args)
{
	return cmd(args);
}

typedef struct PipeSegmentMeta {
	char* cmd;
	char* first_arg;
} PipeSegmentMeta;

typedef struct PipeSegments {
	PipeSegmentMeta* metas;
	int size;
} PipeSegments ;

struct PipeSegmentMeta inner_tokenize(char* start) {
	PipeSegmentMeta cmd_meta = {
		strtok(start, TOKENS),
		strtok(NULL, TOKENS)
	};
	return cmd_meta;
}

PipeSegments* new_pipe_segments(char* line, int max_segments) {
	PipeSegments* psegs = malloc(sizeof(PipeSegments));
	psegs->metas = malloc(max_segments * sizeof(PipeSegmentMeta));
	psegs->size = 0;

	char* segments[max_segments];
	for(char* token = strtok(line, PIPE_TOKEN);
	    token;
	    token = strtok(NULL, PIPE_TOKEN),
	    ++psegs->size)
	{
		segments[psegs->size] = token;
	}

	for(int i = 0; i < psegs->size; ++i) {
		psegs->metas[i] = inner_tokenize(segments[i]);
	}

	return psegs;
}

void free_pipe_segments(PipeSegments* ps) {
	free(ps->metas);
	free(ps);
}

int main()
{

	while (true) {
		char* ret = input(PROMPT);
		if (!ret) {
			perror("`input` returned NULL");
			break;
		}

		if (inbuffer[strlen(inbuffer) - 1] != '\n') {
			fprintf(stderr, "overread");
			break;
		}

		PipeSegments* pipe_segs = new_pipe_segments(inbuffer, 10);

		for(int i = 0; i < pipe_segs->size; ++i){
			printf("%s %s\n", pipe_segs->metas[i].cmd, pipe_segs->metas[i].first_arg);
		}

		free_pipe_segments(pipe_segs);
		break;
	}
	return 0;
}
