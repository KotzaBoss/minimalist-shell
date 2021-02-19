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
	return getcwd(outbuffer, 60) ? 1 : 0;
}

int cd(char* path)
{
	return chdir(path);
}

int ls(char* path)
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

void setup() {
//builtin_names[0] = "pwd";
//	builtin_names[1] = "cd";
//	builtin_names[2] = "ls";
//builtins_func[0] = &pwd;
//	builtins_func[1] = &cd;
//	builtins_func[2] = &ls;
}



