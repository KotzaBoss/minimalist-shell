//
// Created by kotzaboss on 22/02/2021.
//

#ifndef RUN_H
#define RUN_H

#include "interpret.h"
#include "builtins.h"

/**
 * @brief Spawn child, connect pipes to STDIN or STDOUT and execute cmd.
 * @param infd Input file descriptor.
 * @param outfd Output file descriptor.
 */
int run_cmd_in_fork(int infd, int outfd, CMD cmd);
int run_builtin(int infd, int outfd, builtin func, char** args);

#endif //RUN_H
