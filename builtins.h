//
// Created by kotzaboss on 20/02/2021.
//

#ifndef BUILTINS_H
#define BUILTINS_H

#define TOTAL_BUILTINS 3

//const char* builtin_names[TOTAL_BUILTINS];
//int (* builtins_func[TOTAL_BUILTINS])(char*);

int pwd(char* outbuffer);
int cd(char* path);
int ls(char* path);
void setup();

#endif //BUILTINS_H
