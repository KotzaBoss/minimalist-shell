#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pwd.h>
#include "builtins.h"


int builtin_pwd(const char** args)
{
	char buff[100] = "\0";
	if(getcwd(buff, 100)) {
		printf("%s\n", buff);
		return 0;
	}
	return 1;  // Unfortunate reversal. NULL should return 1
}

int builtin_cd(const char** args)
{
	const char* dir = args[1];
	if(!dir){
		dir = getenv("HOME");
		if(!dir) {
			dir = getpwuid(getuid())->pw_name;
		}
	}
	return chdir(dir);
}

int builtin_ls(const char** args)
{
	DIR* dir_stream = opendir(args[1] ? args[1] : ".");
	struct dirent* dir;
	if (dir_stream) {
		while ((dir = readdir(dir_stream))) {
			printf("%s\n", dir->d_name);
		}
		return closedir(dir_stream);
	}
	return 0;
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

static
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
