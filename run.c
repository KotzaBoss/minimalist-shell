#include <unistd.h>
#include <stdlib.h>
#include "interpret.h"
#include "builtins.h"
#include "run.h"

int fork_exec_binary(int infd, int outfd, CMD cmd)
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
		return execvp(CMD_release(cmd)[0], (char* const*)CMD_release(cmd));
	}
	return cpid;
}

int fork_call_builtin(int infd, int outfd, builtin func, CMD cmd)
{
	int cpid = getpid();
	if (func != builtin_cd) {
		cpid = fork();
		if (!cpid) {
			if (infd != STDIN_FILENO) {  // reroute input of pipe
				dup2(infd, STDIN_FILENO);
				close(infd);
			}
			if (outfd != STDOUT_FILENO) {  // rerout output of pipe
				dup2(outfd, STDOUT_FILENO);
				close(outfd);
			}
			exit(func(CMD_release(cmd)));
		}
		return cpid;
	}
	func(CMD_release(cmd));
	return cpid;
}
