#ifndef BUILTINS_H
#define BUILTINS_H

typedef int (*builtin)(const char** args);

/**
 * @brief Search for builtin with name NAME and return that function.
 */
builtin get_func(const char* name);

int builtin_pwd(const char** args);
int builtin_cd(const char** args);
int builtin_ls(const char** args);

#endif //BUILTINS_H
