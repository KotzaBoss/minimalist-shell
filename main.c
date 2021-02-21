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

#define PROMPT ">>> "
#define IN_BUFF_SIZE 64

char uinbuffer[IN_BUFF_SIZE];

char* input(const char* prompt)
{
	printf("%s", prompt);
	return fgets(uinbuffer, IN_BUFF_SIZE, stdin);
}

/**
 * @brief Spawn child connect pipes to STDIN or STDOUT and execute cmd.
 * @param infd Input file descriptor.
 * @param outfd Output file descriptor.
 * @return
 */
int new_child(int infd, int outfd, CMD cmd)
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
		return execvp(cmd_release(cmd)[0], cmd_release(cmd));
	}
	return cpid;
}

int main()
{
	while (true) {
		fflush(stdin);
		char* uin = input(PROMPT);
		if (!uin) {
			perror("`input` returned NULL");
			return 1;
		}
		if (uinbuffer[strlen(uinbuffer) - 1] != '\n') {
			fprintf(stderr, "overread");
			return 1;
		}

		PipeSegments pipe_segs = new_pipe_segments(uinbuffer, 10);

		int fd[2];
		int infd = STDIN_FILENO;
		char* cmd;
		StringList args;
		CMD arg_list;
		for (int i = 0; i < pipesegments_size(pipe_segs) - 1;
		     ++i) {  // last cmd must be outside for
			arg_list = cmd_new(pipesegments_metas(pipe_segs)[i]);

			pipe(fd);
			int cpid = new_child(infd, fd[1], arg_list);
			waitpid(cpid, NULL, 0);
			close(fd[1]);
			infd = fd[0];

			cmd_free(arg_list);
		}

		if (infd != STDIN_FILENO) {
			dup2(infd, STDIN_FILENO);
			close(infd);
		}
		arg_list = cmd_new(pipesegments_metas(pipe_segs)[pipesegments_size(pipe_segs) - 1]);
		int cpid = new_child(infd, fd[1], arg_list);
		waitpid(cpid, NULL, 0);
//		execvp(cmd_release(arg_list)[0], cmd_release(arg_list));
		cmd_free(arg_list);
	}

	/*
	while (true) {
		char* uin = input(PROMPT);

		if (!uin) {
			perror("`input` returned NULL");
			break;
		}
		if (uinbuffer[strlen(uinbuffer) - 1] != '\n') {
			fprintf(stderr, "overread");
			break;
		}

		struct PipeSegments* pipe_segs = new_pipe_segments(uinbuffer, 10);

		char* cmd;
		StringList args;
		for (int i = 0; i < pipe_segs->size; ++i) {
			cmd = pipe_segs->metas[i].cmd;
			args = pipe_segs->metas[i].args;

			char joined_args[ARG_BUFF_SIZE] = "\0";
			if (!strcmp(cmd, "ls")) {
				if (!stringlist_size(args))
					joined_args[0] = '.';
				else
					stringlist_join(args, joined_args, ARG_BUFF_SIZE, " ");

				int nchars = ls(joined_args);
				if (nchars == -1)
					perror("ls");
				else {
					char buff[nchars]
					write(STDIN_FILENO,)
				}
			}
			else if (!strcmp(cmd, "cd")) {
				char* cd_args;
				if (!stringlist_size(args)) {
					if (!(cd_args = getenv("HOME")))
						cd_args = getpwuid(getuid())->pw_dir;
				}
				else {
					stringlist_join(args, joined_args, ARG_BUFF_SIZE, " ");
					cd_args = joined_args;
				}

				if (cd(cd_args))
					perror("cd");
			}
			else if (!strcmp(cmd, "pwd")) {
				if (!pwd(cmd, CWD_BUFF_SIZE))
					perror("pwd");
			}
			else {  // Assume cmd is binary
				int arg_list_len = 1 + stringlist_size(args) + 1;  // cmd + args + NULL
				char* arg_list[arg_list_len];

				arg_list[0] = cmd;
				for (int i = 1; i < arg_list_len - 1; ++i)
					arg_list[i] = stringlist_at(args, i - 1);
				arg_list[arg_list_len - 1] = NULL;  // {"cmd", "arg1", "args2", ..., NULL}

				int pid = fork();
				if (pid == 0) {  // in child
					int exec_code = execvp(cmd, arg_list);
					_exit(exec_code);
				}
				else {
					printf("parent waiting\n");
					wait(NULL);
					printf("child dead\n");
				}
			}
		}

		free_pipe_segments(pipe_segs);
	}
	return 0;*/
}
