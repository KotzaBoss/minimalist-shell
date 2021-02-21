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


char* pwd(char* outbuffer, int size)
{
	return getcwd(outbuffer, size);
}

int cd(char* path)
{
	return chdir(path);
}

int ls(char* path)
{
	DIR* dir_stream = opendir(path);
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

const char* builtins[TOTAL_BUILTINS] = {
	"pwd",
	"cd",
	"ls"
};

int is_builtin(const char* cmd) {
	for(int i = 0; i < TOTAL_BUILTINS; ++i) {
		if(!strcmp(builtins[i], cmd)) {
			return i;
		}
	}
	return -1;
}

//int get_cmd(const char* name){
//	void* f = &ls;
//
//	char* s = ".";
//	(int(*)(char*)f)(s);
//	for(int i = 0; i < TOTAL_BUILTINS; ++i) {
//		if(is_builtin(name) >= 0){
//
//			return builtin_funcs[i];
//		}
//	}
//	return NULL;
//}

void setup()
{
//	builtin_names[0] = "pwd";
//	builtin_names[1] = "cd";
//	builtin_names[2] = "ls";
//builtins_func[0] = &pwd;
//	builtins_func[1] = &cd;
//	builtins_func[2] = &ls;
}



