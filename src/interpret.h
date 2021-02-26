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

/**
 * @brief Allocate memory for a new PipeSegments obj.
 * CAUTION: Line should be sanatized through escape first.
 */
PipeSegmentMeta PipeSegmentMeta_new  ();
void            PipeSegmentMeta_free (PipeSegmentMeta* psm);

/**
 * @brief Parse LINE and create new PipeSegments. CAUTION: LINE should be sanatized
 * through main.c:input before use.
 * Parsing consists of a few steps
 * 1. Find & and set wait flag
 * 2. Tokenize the line into "pipe segments"
 * 3. Tokenize each of these segments and create a StringList of each token
 *    at PipeSegments->metas->args.
 */
PipeSegments     PipeSegments_new   (char* line, int max_segments);
void             PipeSegments_free  (PipeSegments* ps);

/**
 * @return The size field of PipeSegments
 */
int              PipeSegments_size  (PipeSegments ps);

/**
 * @return The metas field of PipeSegments
 */
PipeSegmentMeta* PipeSegments_metas (PipeSegments ps);

/**
 * @return Last pipe segment
 */
PipeSegmentMeta  PipeSegments_last  (PipeSegments ps);

/**
 * @return The wait field of PipeSegment
 */
bool             PipeSegments_wait  (PipeSegments ps);

/**
 * @brief Create a new CMD. Its total length is command + number_of_args + NULL.
 * NULL is very important for execvp.
 */
CMD    CMD_new     (PipeSegmentMeta psm);
void   CMD_free    (CMD* cmd);

/**
 * @brief Return the encapsulated const char** array.
 */
const char** CMD_release (CMD cmd);

/**
 * @brief Return the name of the command, which is always the first.
 * eg: {"echo", "1"}[0], {"grep", "abc", "./file,txt"}[0]
 */
const char* CMD_name (CMD cmd);

/**
 * @brief Shell like expansion of buffer which is *overwritten*.
 * If resulting string size is greater that buffer size return NULL else
 * return the pointer to the buffer.
 * @param buffer Buffer scanned and overwritten.
 */
char* expand_buffer(char* buffer, int max_size);

#endif //INTERPRET_H
