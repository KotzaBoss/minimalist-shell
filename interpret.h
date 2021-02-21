//
// Created by kotzaboss on 20/02/2021.
//

#ifndef INTERPRET_H
#define INTERPRET_H

#include "stringlist.h"

#define PIPE_TOKENS "|\n"
#define CMD_TOKENS " \n"

typedef struct PipeSegmentMeta* PipeSegmentMeta;
typedef struct PipeSegments* PipeSegments;
typedef struct CMD* CMD;

PipeSegmentMeta new_pipesegmentmeta();
void free_pipesegmentmeta(PipeSegmentMeta);

PipeSegments new_pipe_segments(char* line, int max_segments);
int pipesegments_size(PipeSegments ps);
PipeSegmentMeta* pipesegments_metas(PipeSegments ps);
void free_pipe_segments(PipeSegments ps);

CMD cmd_new(PipeSegmentMeta psm);
char** cmd_release(CMD);
void cmd_free(CMD cmd);

void prepare_cmd(PipeSegmentMeta psm, char** dest);
#endif //INTERPRET_H
