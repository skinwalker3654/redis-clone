#include "../header/commands.h"
#include <string.h>
#include <stdio.h>

Command commands_table[] = {
    {"set",op_set},
    {"get",op_get},
    {"del",op_del},
    {"rpush",op_rpush},
    {"exists",op_exists},
    {"flushall",op_flushall},
    {"clear",op_clear},
    {"lrange",op_lrange},
    {"ttl",op_ttl},
    {"expire",op_expire},
    {"rpop",op_rpop},
    {"lset",op_lset},
    {"lindex",op_lindex},
    {"help",op_help},
    {NULL,NULL},
};

command_func find_command(char *name) {
    for(int i=0; commands_table[i].name != NULL; i++) {
        if(strcmp(commands_table[i].name,name)==0) {
            return commands_table[i].func;
        }
    }

    return NULL;
}

int execute(HashMap **map, command_t *comm) {
    command_func func = find_command(comm->argv[0]);
    if(!func) {
        printf("error uknown command '%s'\n",comm->argv[0]);
        return -1;
    }

    func(map,comm);
    return 0;
}
