#ifndef BUILTINS_H
#define BUILTINS_H

typedef int (*builtin)(const char** args);

builtin get_func(const char* name);
int builtin_cd(const char** args);

#endif //BUILTINS_H
