#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <wordexp.h>
#include <sys/wait.h>
#include <errno.h>

#include "builtins.h"
#include "interpret.h"
#include "run.h"

#define PROMPT ">>> "
#define IN_BUFF_SIZE 1024

#define PERROR(str) \
    fprintf(stderr, "~%s:%d: ", __FILE__, __LINE__); \
    perror(str);

static
void cleanup(PipeSegments* ps, CMD* cmd)
{
	if (*ps) {
		PipeSegments_free(ps);
	}
	if (*cmd) {
		CMD_free(cmd);
	}
}

char uinbuffer[IN_BUFF_SIZE];

static
char* input(const char* prompt)
{
	printf("%s ", prompt);
	char* uin = fgets(uinbuffer, IN_BUFF_SIZE, stdin);
	for (size_t i = strlen(uin) - 1;
	     i >= 0 && (uin[i] == '\n' || uin[i] == ' ');
	     --i) {  // strip write whitespace and new line
		uin[i] = '\0';
	}
	return uin;
}

int main()
{
	int fd[2], infd, outfd, pid, cpid;
	char* uin;
	builtin bfunc;
	// Free on Error
	PipeSegments pipe_segs = NULL;
	CMD cmd = NULL;

	while (true) {
	WhileTop:
		memset(uinbuffer, 0, sizeof(uinbuffer));  // Aggressive reset

		uin = input(PROMPT);
		if (!uin) {
			PERROR("`input` returned NULL");
			continue;
		}
		if (!strcmp(uin, "\n")) {  // Only enter pressed
			continue;
		}
		if (!strcmp(uin, "exit")) {
			break;
		}
		if (!expand_buffer(uin, IN_BUFF_SIZE)) {
			PERROR("Couldnt expand user input.");
			continue;
		}

		pid = getpid();
		pipe_segs = PipeSegments_new(uinbuffer, 10);
		if (!PipeSegments_wait(pipe_segs)) {  // & at end of phrase
			pid = fork();
			if (pid == -1) {
				PERROR("& process couldnt be forked.");
				PipeSegments_free(&pipe_segs);
				continue;
			}
			if (pid) {
				printf("Starting job %d\n", pid);
				continue;
			}
		}  // Child continues

		infd = STDIN_FILENO;
		outfd = STDOUT_FILENO;
		for (int i = 0; i < PipeSegments_size(pipe_segs) - 1; ++i) { // last cmd must be outside for
			cmd = CMD_new(PipeSegments_metas(pipe_segs)[i]);

			pipe(fd);
			outfd = fd[1];  // A bit verbose FIX later
			if ((bfunc = get_func(CMD_name(cmd)))) {
				cpid = fork_call_builtin(infd, outfd, bfunc, cmd);
			}
			else {
				cpid = fork_exec_binary(infd, outfd, cmd);
				if (cpid == -1) {
					PERROR("Binary couldnt be forked.");
					cleanup(&pipe_segs, &cmd);
					goto WhileTop;
				}
			}
			wait_set_errno(cpid);

			close(outfd);  // 1000 iq close
			infd = fd[0];  // write-to pipe will be used in the next iteration

			CMD_free(&cmd);
		}  // infd = pipe, outfd = stdout (from fork_exec_binary)

		cmd = CMD_new(PipeSegments_last(pipe_segs));  // Code repetition but it works...
		if ((bfunc = get_func(CMD_name(cmd)))) {
			cpid = fork_call_builtin(infd, outfd, bfunc, cmd);
		}
		else {
			cpid = fork_exec_binary(infd, outfd, cmd);
			if (cpid == -1) {
				PERROR("Binary could be forked.");
				cleanup(&pipe_segs, &cmd);
				goto WhileTop;
			}
		}
		wait_set_errno(cpid);

		cleanup(&pipe_segs, &cmd);
		if (!pid) {
			printf("Job done %d\n%s", getpid(), PROMPT);
			exit(1);
		}

	}

	cleanup(&pipe_segs, &cmd);
	return EXIT_SUCCESS;
}
