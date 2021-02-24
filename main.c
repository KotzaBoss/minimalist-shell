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
#include <wordexp.h>

#define GOTO_ERROR(str) \
    fprintf(stderr, "%s:%d: ", __FILE__, __LINE__); \
    perror(str); \
    goto Error

#define PROMPT ">>> "
#define IN_BUFF_SIZE 1024

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

/**
 * @brief Overwrite buffer adding escape backslash for the esc_chr characters
 */
static
void escape(char* buffer, int max_size, const char* esc_chr)
{
	char* tmp = malloc(max_size * sizeof(char));
	for (int i = 0, buff_i = 0; i < max_size; ++i, ++buff_i) {
		for (int j = 0; j < strlen(esc_chr); ++j) {
			if (esc_chr[j] == buffer[buff_i]) {
				tmp[i++] = '\\';
			}
			tmp[i] = buffer[buff_i];
		}
	}
	for (int i = 0; i < max_size; ++i) {
		buffer[i] = tmp[i];
	}
	free(tmp);
}

/**
 * @brief Shell like expansion of buffer which is *overwritten*.
 * If resulting string size is greater that buffer size return NULL else
 * return the pointer to the buffer.
 * @param buffer Buffer scanned and overwritten.
 */
char* expand_buffer(char* buffer, int max_size)
{
	buffer[strcspn(buffer, "\n")] = '\0';  // wordexp gets confused with "\n"
	escape(buffer, max_size, "|");

	wordexp_t w;
	wordexp(uinbuffer, &w, 0);
	size_t expanded_length = 0;
	for (int i = 0; i < w.we_wordc; ++i) {
		expanded_length += strlen(w.we_wordv[i]) + 1;  // + space
	}
	++expanded_length; // NULL termination
	if (expanded_length > max_size) {
		return NULL;
	}

	int buff_i = 0;
	for (int w_str = 0; w_str < w.we_wordc; ++w_str) {
		for (int w_str_i = 0; w_str_i < strlen(w.we_wordv[w_str]); ++w_str_i, ++buff_i) {
			buffer[buff_i] = w.we_wordv[w_str][w_str_i];
		}
		buffer[buff_i++] = ' ';
	}

	wordfree(&w);
	return buffer;
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

		if (!strcmp(uin, "\n")) {  // Only enter pressed
			continue;
		}
		if (!strcmp(uin, "exit")) {
			break;
		}
		if (!expand_buffer(uin, IN_BUFF_SIZE)) {
			continue;
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
