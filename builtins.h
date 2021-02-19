//
// Created by kotzaboss on 20/02/2021.
//

#ifndef BUILTINS_H
#define BUILTINS_H

const char* const builtin_names[];
int (* builtins_func[])(const char*);

int pwd(char* outbuffer);
int cd(const char* path);
int ls(const char* path);

#endif //BUILTINS_H
