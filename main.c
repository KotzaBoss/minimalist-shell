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

void cleanup(PipeSegments* ps, CMD* cmd)
{
	PipeSegments_free(ps);
	CMD_free(cmd);
}

#define GOTO_AFTER_CLEANUP(to, ps, cmd) \
    cleanup(ps, cmd); \
    goto to;

#define GOTO_ERROR(str) \
    fprintf(stderr, "%s:%d: ", __FILE__, __LINE__); \
    perror(str); \
    goto Error

#define PROMPT ">>> "
#define IN_BUFF_SIZE 64

char uinbuffer[IN_BUFF_SIZE];

char* input(const char* prompt)
{
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
	int pid;

	while (true) {
	WhileTop:
		printf("%s", PROMPT);
		uin = input(PROMPT);
		if (!uin) {
			GOTO_ERROR("`input` returned NULL");
		}
		if (uinbuffer[strlen(uinbuffer) - 1] != '\n') {
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
		pid = getpid();
		if (!PipeSegments_wait(pipe_segs)) {  // & at end of phrase
			pid = fork();
			if (pid == -1) {
				PipeSegments_free(&pipe_segs);
				goto WhileTop;
			}
			if (pid) {
				printf("Starting job %d\n", pid);
			}
		}

		for (int i = 0; i < PipeSegments_size(pipe_segs) - 1; ++i) { // last cmd must be outside for
			cmd = CMD_new(PipeSegments_metas(pipe_segs)[i]);

			pipe(fd);
			outfd = fd[1];  // A bit verbose FIX later
			if ((bfunc = get_func(CMD_name(cmd)))) {
				cpid = run_builtin(infd, outfd, bfunc, CMD_release(cmd));
			}
			else {
				cpid = run_cmd_in_fork(infd, outfd, cmd);
				if (cpid == -1) {
					PipeSegments_free(&pipe_segs);
					CMD_free(&cmd);
					goto WhileTop;
				}
				waitpid(cpid, NULL, 0);
			}

			close(outfd);
			infd = fd[0];  // write-to pipe will be used in the next iteration

			CMD_free(&cmd);
		}  // infd = pipe, outfd = stdout (from run_cmd_in_fork)

		cmd = CMD_new(PipeSegments_last(pipe_segs));  // Code repetition but it works...
		if ((bfunc = get_func(CMD_name(cmd)))) {
			run_builtin(infd, outfd, bfunc, CMD_release(cmd));
		}
		else {
			cpid = run_cmd_in_fork(infd, outfd, cmd);
			if (cpid == -1) {
				PipeSegments_free(&pipe_segs);
				CMD_free(&cmd);
				goto WhileTop;
			}
			waitpid(cpid, NULL, 0);
		}

		CMD_free(&cmd);
		PipeSegments_free(&pipe_segs);

		if (!pid) {
			printf("Job done %d\n", getpid());
			printf("%s", PROMPT);  // Hacky?
			exit(1);
		}
	}

Error:
	if (pipe_segs) {
		PipeSegments_free(&pipe_segs);
	}
	if (cmd) {
		CMD_free(&cmd);  // TODO cmd->cmd not malloced???
	}
	return 1;
}
