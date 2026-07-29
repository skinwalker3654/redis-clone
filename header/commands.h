#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdlib.h>
#include "operations.h"

typedef int (*command_func)(HashMap **map, command_t *cmd, execute_type type);

typedef struct {
    char *name;
    command_func func;
} Command;

extern Command commands_table[];

command_func find_command(char *name);
int execute(HashMap **map, command_t *comm, execute_type type);

#endif
