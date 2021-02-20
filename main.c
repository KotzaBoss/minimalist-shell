#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/wait.h>
#include "builtins.h"
#include "interpret.h"

#define PROMPT ">>> "
#define IN_BUFF_SIZE 64
#define CWD_BUFF_SIZE 100
#define ARG_BUFF_SIZE 100

#define CMD_WIDTH 15

char inbuffer[IN_BUFF_SIZE];

char cwd[CWD_BUFF_SIZE];

char* input(const char* prompt)
{
	printf("%s", prompt);
	return fgets(inbuffer, IN_BUFF_SIZE, stdin);
}

//int run(funcsig cmd, char* args)
//{
//	return cmd(args);
//}

int main()
{
	while (true) {
		char* ret = input(PROMPT);
		if (!ret) {
			perror("`input` returned NULL");
			break;
		}

		if (inbuffer[strlen(inbuffer) - 1] != '\n') {
			fprintf(stderr, "overread");
			break;
		}

		struct PipeSegments* pipe_segs = new_pipe_segments(inbuffer, 10);

//		for (int i = 0; i < stringlist_size(pipe_segs->metas->args); ++i) {
//			printf("%s\n", stringlist_at(pipe_segs->metas->args, i));
//		}
//		printf("%s\n", pipe_segs->metas->cmd);

		char* cmd;
		StringList args;
		for (int i = 0; i < pipe_segs->size; ++i) {
			cmd = pipe_segs->metas[i].cmd;
			args = pipe_segs->metas[i].args;

			char joined_args[ARG_BUFF_SIZE] = "\0";
			if (!strcmp(cmd, "ls")) {
				if (!stringlist_size(args)) {
					joined_args[0] = '.';
				}
				else {
					stringlist_join(args, joined_args, ARG_BUFF_SIZE, " ");
				}

				if (ls(joined_args)) {
					perror("ls");
				}
			}
			else if (!strcmp(cmd, "cd")) {
				char* cd_args;

				if (!stringlist_size(args)) {
					if (!(cd_args = getenv("HOME"))) {
						cd_args = getpwuid(getuid())->pw_dir;
					}
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
			else {
//				printf("before fork\n");
				int arg_list_len = 1 + stringlist_size(args) + 1;
				char* arg_list[arg_list_len];
				arg_list[0] = cmd;
				for (int i = 1; i < arg_list_len - 1; ++i) {
					arg_list[i] = stringlist_at(args, i-1);
				}
				arg_list[arg_list_len-1] = NULL;  // exec requires last element to be NULL

//				for(int i = 0; i < arg_list_len; ++i)
//					printf("%s\n", arg_list[i] ? arg_list[i] : "null");
				int pid = fork();
				if (pid == 0) {  // in child
					int i = execvp(cmd, arg_list);
//					printf("%d\n", i);
					_exit(0);
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
	return 0;
}
