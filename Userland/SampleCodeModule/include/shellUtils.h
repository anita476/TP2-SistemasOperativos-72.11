#ifndef _SHELL_UTILS_H
#define _SHELL_UTILS_H

#include <libSysCalls.h>
#include <shell.h>

/*
    @brief Tokenizes a command
*/
int tokenize(char *buffer, char **args);

#endif