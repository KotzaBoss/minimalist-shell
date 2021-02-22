#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/wait.h>
#include <assert.h>
#include "builtins.h"
#include "interpret.h"
#include "run.h"

#define GOTO_ERROR(str) \
    perror(str); \
    goto Error

#define PROMPT ">>> "
#define IN_BUFF_SIZE 64

char uinbuffer[IN_BUFF_SIZE];

char* input(const char* prompt)
{
	printf("%s", prompt);
	return fgets(uinbuffer, IN_BUFF_SIZE, stdin);
}

int main()
{
	// Free on Error
	PipeSegments pipe_segs = NULL;
	CMD arg_list = NULL;

	int fd[2];
	int infd = STDIN_FILENO;
	int outfd;
	char* uin;
	int cpid;
	builtin bfunc;
	while (true) {
		uin = input(PROMPT);
		if (!uin) {
			GOTO_ERROR("`input` returned NULL");
		}
		if (uinbuffer[strlen(uinbuffer) - 1] != '\n') {
			GOTO_ERROR("Overread");
		}

		pipe_segs = PipeSegments_new(uinbuffer, 10);

		for (int i = 0; i < PipeSegments_size(pipe_segs) - 1; ++i) { // last cmd must be outside for
			arg_list = CMD_new(PipeSegments_metas(pipe_segs)[i]);

			pipe(fd);
			outfd = fd[1];
			if ((bfunc = get_func(CMD_name(arg_list)))) {
				fprintf(stderr, "builtin");
				cpid = run_builtin_in_fork(infd, outfd, bfunc, CMD_release(arg_list));
			}
			else {
				fprintf(stderr, "foreign binary");
				cpid = run_cmd_in_fork(infd, outfd, arg_list);
			}
			waitpid(cpid, NULL, 0);
			close(outfd);
			infd = fd[0];  // Pipe will be used in the next iteration

			CMD_free(&arg_list);
		}  // infd = pipe, outfd = stdout (from run_cmd_in_fork)

		if (infd != STDIN_FILENO) {
			dup2(infd, STDIN_FILENO);
			close(infd);
		}

		// TODO fix crash of fgets
		arg_list = CMD_new(PipeSegments_metas(pipe_segs)[PipeSegments_size(pipe_segs) - 1]);
		if ((bfunc = get_func(CMD_name(arg_list)))) {
			fprintf(stderr, "builtin\n");
			cpid = run_builtin_in_fork(infd, outfd, bfunc, CMD_release(arg_list));
		}
		else {
			fprintf(stderr, "foreign binary");
			cpid = run_cmd_in_fork(infd, outfd, arg_list);
		}
//		int cpid = fork();
//		if(!cpid) {
//			execvp(CMD_release(arg_list)[0], CMD_release(arg_list));
//		}
		waitpid(cpid, NULL, 0);
		CMD_free(&arg_list);
		PipeSegments_free(&pipe_segs);
	}

Error:
	if (pipe_segs)
		PipeSegments_free(&pipe_segs);
	if (arg_list)
		CMD_free(&arg_list);  // TODO cmd->cmd not malloced???
	return 1;
}
