#include <unistd.h>
#include <stdlib.h>
#include <wait.h>
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


int wait_set_errno(int id)
{
	int exec_code, cmd_code = 0;
	waitpid(id, &exec_code, 0);
	if(WIFEXITED(exec_code)) {
		errno = cmd_code = WEXITSTATUS(exec_code);
	}
	return cmd_code;

}
