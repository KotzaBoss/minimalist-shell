//
// Created by kotzaboss on 22/02/2021.
//

#include "run.h"
#include "interpret.h"
#include "builtins.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int run_cmd_in_fork(int infd, int outfd, CMD cmd)
{
	int cpid = fork();
	if (!cpid) {
		if (infd != STDIN_FILENO) {  // reroute input of pipe
			dup2(infd, STDIN_FILENO);
			close(infd);
		}
		if (outfd != STDOUT_FILENO) {  // rerout output of pipe
			dup2(outfd, STDOUT_FILENO);
			close(outfd);
		}
		return execvp(CMD_release(cmd)[0], CMD_release(cmd));
	}
	return cpid;
}

int run_builtin_in_fork(int infd, int outfd, builtin func, char** args)
{
	if (infd != STDIN_FILENO) {  // reroute input of pipe
		dup2(infd, STDIN_FILENO);
		close(infd);
	}
	if (outfd != STDOUT_FILENO) {  // rerout output of pipe
		dup2(outfd, STDOUT_FILENO);
		close(outfd);
	}
	return func(args, infd, outfd);
}