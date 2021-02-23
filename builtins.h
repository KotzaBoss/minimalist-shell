//
// Created by kotzaboss on 20/02/2021.
//

#ifndef BUILTINS_H
#define BUILTINS_H

typedef int (*builtin)(char**, int, int);

builtin get_func(const char* name);
int builtin_cd(char** args, int, int);

#endif //BUILTINS_H
