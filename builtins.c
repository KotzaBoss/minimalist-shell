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

int pwd(char* outbuffer)
{
	return getcwd(outbuffer, 5000) ? 1 : 0;
}

int cd(const char* path)
{
	return chdir(path);
}

int ls(const char* path)
{
	DIR* dir_stream = opendir(path);
	struct dirent* dir;
	if (dir_stream) {
		while ((dir = readdir(dir_stream))) {
			printf("%s\n", dir->d_name);
		}
		closedir(dir_stream);
		return 0;
	}
	return 1;
}

const char* const builtin_names[] = {
	"pwd",
	"cd",
	"ls"
};

int (* builtins_func[])(const char*) = {
	&pwd,
	&cd,
	&ls
};
