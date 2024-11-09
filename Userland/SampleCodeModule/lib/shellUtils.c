// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <shell.h>
#include <shellUtils.h>

int tokenize(char *buffer, char **args) {
  int argc = 0;
  char *current = buffer;
  args[argc++] = current;

  while (*current) {
    if (*current == ' ') {
      *current = '\0';
      current++;
      if (*current && argc < BUFFER_SIZE) {
        args[argc++] = current;
      }
    } else {
      current++;
    }
  }
  return argc;
}
