//
// Created by kotzaboss on 20/02/2021.
//

#ifndef BUILTINS_H
#define BUILTINS_H

#define TOTAL_BUILTINS 3

//const char* builtin_names[TOTAL_BUILTINS];

//int (* builtins_func[TOTAL_BUILTINS])(char*);

char* pwd(char* outbuffer, int size);
int cd(char* path);
int ls(char* path);

int is_builtin(const char* cmd);
int get_cmd(const char* name);

void setup();

#endif //BUILTINS_H
