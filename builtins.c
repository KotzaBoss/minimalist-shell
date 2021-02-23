//
// Created by kotzaboss on 20/02/2021.
//

#include <unistd.h>
#include <stdbool.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "builtins.h"
#include <pwd.h>


int builtin_pwd(char** args, int infd, int outfd)
{
	char buff[100] = "\0";
	if(getcwd(buff, 100)) {
		printf("%s\n", buff);
		return 1;
	}
	return 0;
}


// TODO: doesnt work with .. (and in general)
int builtin_cd(char** args, int infd, int outfd)
{
	char* dir = args[1];
	if(!dir){
		dir = getenv("HOME");
		if(!dir) {
			dir = getpwuid(getuid())->pw_name;
		}
	}
	return chdir(dir);
}

int builtin_ls(char** args, int infd, int outfd)
{
	DIR* dir_stream = opendir(args[1] ? args[1] : ".");
	struct dirent* dir;
	int len = -1;
	if (dir_stream) {
		len = 0;
		while ((dir = readdir(dir_stream))) {
			printf("%s\n", dir->d_name);
			len += strlen(dir->d_name);
		}
		closedir(dir_stream);
	}
	return len;
}

const char* names[] = {
	"pwd",
	"cd",
	"ls"
};

builtin func[] = {
	&builtin_pwd,
	&builtin_cd,
	&builtin_ls
};

int total_builtins() {
	return sizeof(names) / sizeof(char*);  // note to self: total bytes / elem type
}

builtin get_func(const char* name) {
	for(int i = 0; i < total_builtins(); ++i) {
		if(!strcmp(names[i], name)) {
			return func[i];
		}
	}
	return NULL;
}
