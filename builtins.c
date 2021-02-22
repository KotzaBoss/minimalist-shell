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


int builtin_pwd(char** args)
{
	return getcwd(args[0], strtol(args[1], NULL, 10)) ? 1 : 0;
}

int builtin_cd(char** args)
{
	char* dir = args[1];
	if(!dir){
		dir = getenv("HOME");
		if(!dir) {
			dir = getpwuid(getuid())->pw_name;
		}
	}
	fprintf(stderr, "%s\n", dir ? dir : "null");
	return chdir(dir);
}

int builtin_ls(char** args)
{
	DIR* dir_stream = opendir(args[0]);
	struct dirent* dir;
	int len = 0;
	if (dir_stream) {
		while ((dir = readdir(dir_stream))) {
			printf("%s\n", dir->d_name);
			len += strlen(dir->d_name);
		}
		closedir(dir_stream);
		return len;
	}
	return -1;
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
