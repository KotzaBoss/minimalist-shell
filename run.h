#ifndef RUN_H
#define RUN_H

#include "interpret.h"
#include "builtins.h"

/**
 * @brief Spawn child, connect pipes to STDIN or STDOUT and execute cmd.
 * @param infd Input file descriptor.
 * @param outfd Output file descriptor.
 */
int fork_exec_binary(int infd, int outfd, CMD cmd);

/**
 * @brief Run func with cmd arguments. If the function is builtin_cd the *dont* fork.
 * @param infd Input file description
 * @param outfd Output file descriptor
 */
int fork_call_builtin(int infd, int outfd, builtin func, CMD cmd);

#endif //RUN_H
