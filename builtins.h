//
// Created by kotzaboss on 20/02/2021.
//

#ifndef BUILTINS_H
#define BUILTINS_H

typedef int (*builtin)(char**);

builtin get_func(const char* name);

#endif //BUILTINS_H
