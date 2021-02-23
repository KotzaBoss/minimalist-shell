//
// Created by kotzaboss on 20/02/2021.
//

#ifndef INTERPRET_H
#define INTERPRET_H

#include "stringlist.h"

#define PIPE_TOKENS "|\n"
#define CMD_TOKENS " \n"
#define WAIT_CHAR '&'

/**
 * @brief Type containing pointer to the command string and StringList of arguments.
 */
typedef struct PipeSegmentMeta* PipeSegmentMeta;

/**
 * @brief Type containing an array of PipeSegmentMeta and its size.
 */
typedef struct PipeSegments* PipeSegments;

/**
 * @brief Type encapsulating an array of strings to be used with execvp
 */
typedef struct CMD* CMD;

PipeSegmentMeta PipeSegmentMeta_new  ();
void            PipeSegmentMeta_free (PipeSegmentMeta* psm);

PipeSegments     PipeSegments_new   (char* line, int max_segments);
void             PipeSegments_free  (PipeSegments* ps);
int              PipeSegments_size  (PipeSegments ps);
PipeSegmentMeta* PipeSegments_metas (PipeSegments ps);
PipeSegmentMeta  PipeSegments_last  (PipeSegments ps);
bool             PipeSegments_wait  (PipeSegments ps);

CMD    CMD_new     (PipeSegmentMeta psm);
void   CMD_free    (CMD* cmd);
char** CMD_release (CMD cmd);
const char* CMD_name (CMD cmd);

void prepare_cmd(PipeSegmentMeta psm, char** dest);

#endif //INTERPRET_H
