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

void sigint_handler(int x) {
	fprintf(stderr, "Exiting...\n");
	exit(EXIT_SUCCESS);
}

int main()
{
	signal(SIGINT, sigint_handler);

	// Free on Error
	PipeSegments pipe_segs = NULL;
	CMD arg_list = NULL;

	int fd[2];
	int infd = STDIN_FILENO;
	int outfd = STDOUT_FILENO;
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

		pipe_segs = PipeSegments_new(uinbuffer, 10);

		for (int i = 0; i < PipeSegments_size(pipe_segs) - 1; ++i) { // last cmd must be outside for
			arg_list = CMD_new(PipeSegments_metas(pipe_segs)[i]);

			pipe(fd);
			outfd = fd[1];
			fprintf(stderr, "for %d %d %d-%d\n", infd, outfd, fd[0], fd[1]);
			if ((bfunc = get_func(CMD_name(arg_list)))) {
				fprintf(stderr, "builtin %s\n", CMD_name(arg_list));
				cpid = run_builtin_in_fork(infd, outfd, bfunc, CMD_release(arg_list));
			}
			else {
				fprintf(stderr, "foreign binary IN FOR\n");
				cpid = run_cmd_in_fork(infd, outfd, arg_list);
			}
			if(PipeSegments_wait(pipe_segs)) {
				waitpid(cpid, NULL, 0);
			}
			close(outfd);
			infd = fd[0];  // Pipe will be used in the next iteration

			CMD_free(&arg_list);
		}  // infd = pipe, outfd = stdout (from run_cmd_in_fork)

		arg_list = CMD_new(PipeSegments_last(pipe_segs));
		if ((bfunc = get_func(CMD_name(arg_list)))) {
			fprintf(stderr, "builtin %s\n", CMD_name(arg_list));
			run_builtin_in_fork(infd, outfd, bfunc, CMD_release(arg_list));
		}
		else {
			fprintf(stderr, "foreign binary OUT OF FOR\n");
			cpid = run_cmd_in_fork(infd, outfd, arg_list);
		}
		if(PipeSegments_wait(pipe_segs)){
		waitpid(cpid, NULL, 0);

		}

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
