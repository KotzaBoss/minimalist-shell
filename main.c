#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>
#include <pwd.h>
#include "builtins.h"
#include "interpret.h"

#define PROMPT ">>> "
#define IN_BUFF_SIZE 64
#define CWD_BUFF_SIZE 100

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

		for(int i = 0; i < stringlist_size(pipe_segs->metas->args); ++i) {
			printf("%s\n", stringlist_at(pipe_segs->metas->args, i));
		}

		char* cmd;
		StringList args;
		for(int i = 0; i < pipe_segs->size; ++i){
			cmd = pipe_segs->metas[i].cmd;
			args = pipe_segs->metas[i].args;

			if (!strcmp(cmd, "ls")) {
				char joined_args[10] = "\0";
				if(!stringlist_size(args))
					joined_args[0] = '.';
				else
					stringlist_join(args, joined_args, 10, " ");
				fprintf(stderr, "%-10s ... %s\n", "ls", ls(joined_args) ? "ERROR" : "ok");
			}
			else if (!strcmp(cmd, "cd")) {
				if(!args) {
					if(!(args = getenv("HOME"))){
						args = getpwuid(getuid())->pw_dir;
					}
				}
				fprintf(stderr, "%-10s ... %s\n", "cd", cd(args) ? "ERROR" : "ok");
			}
			else if (!strcmp(cmd, "pwd")){
				fprintf(stderr, "%-10s ... %s %s\n", "pwd", pwd(cwd, CWD_BUFF_SIZE) ? "ok" : "ERROR", cwd);
			}
			else {
				int pid = fork();
				if(!pid) {  // in child
				}

			}
		}

		free_pipe_segments(pipe_segs);
		break;
	}
	return 0;
}
