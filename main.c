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

void sigint_handler(int x)
{
	fprintf(stderr, "Exiting...\n");
	exit(EXIT_SUCCESS);
}

int main()
{
	signal(SIGINT, sigint_handler);

	// Free on Error
	PipeSegments pipe_segs = NULL;
	CMD cmd = NULL;

	int fd[2];
	int infd;
	int outfd;
	char* uin;
	int cpid;
	builtin bfunc;

	while (true) {
		uin = input(PROMPT);
		if (!uin) {
			fprintf(stderr, "from line 45");
			GOTO_ERROR("`input` returned NULL");
		}
		if (uinbuffer[strlen(uinbuffer) - 1] != '\n') {
			fprintf(stderr, "from line 49");
			GOTO_ERROR("Overread");
		}

		if (!strcmp(uin, "\n")) {
			continue;
		}
		if (!strcmp(uin, "exit")) {
			break;
		}

		infd = STDIN_FILENO;
		outfd = STDOUT_FILENO;
		pipe_segs = PipeSegments_new(uinbuffer, 10);
		for (int i = 0; i < PipeSegments_size(pipe_segs) - 1; ++i) { // last cmd must be outside for
			cmd = CMD_new(PipeSegments_metas(pipe_segs)[i]);

			pipe(fd);
			outfd = fd[1];
			if ((bfunc = get_func(CMD_name(cmd)))) {
				cpid = run_builtin_in_fork(infd, outfd, bfunc, CMD_release(cmd));
			}
			else {
				cpid = run_cmd_in_fork(infd, outfd, cmd);
			}

			if (PipeSegments_wait(pipe_segs)) {
				waitpid(cpid, NULL, 0);
			}

			close(outfd);
			infd = fd[0];  // Pipe will be used in the next iteration

			CMD_free(&cmd);
		}  // infd = pipe, outfd = stdout (from run_cmd_in_fork)

		// Code repetition but works...
		cmd = CMD_new(PipeSegments_last(pipe_segs));
		if ((bfunc = get_func(CMD_name(cmd)))) {
			run_builtin_in_fork(infd, outfd, bfunc, CMD_release(cmd));
		}
		else {
			cpid = run_cmd_in_fork(infd, outfd, cmd);
		}

		if (PipeSegments_wait(pipe_segs)) {
			fprintf(stderr, "waiting\n");
			waitpid(cpid, NULL, 0);
		}

		CMD_free(&cmd);
		PipeSegments_free(&pipe_segs);
	}

Error:
	if (pipe_segs)
		PipeSegments_free(&pipe_segs);
	if (cmd)
		CMD_free(&cmd);  // TODO cmd->cmd not malloced???
	return 1;
}
