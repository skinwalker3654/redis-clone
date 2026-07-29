#include "../header/commands.h"
#include "../header/color.h"
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
    {"keys",op_keys},
    {"incrby",op_incrby},
    {"decrby",op_decrby},
    {"rename",op_rename},
    {"setex",op_setex},
    {"pexpireat",op_pexpireat},
    {"type",op_type},
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

int execute(HashMap **map, command_t *comm, execute_type type) {
    command_func func = find_command(comm->argv[0]);
    if(!func) {
        printf(COLOR_RED "\n(error) ERR unknown command '%s'\n" COLOR_RESET, comm->argv[0]);
        printf(COLOR_YELLOW "(error) ERR type 'help' for available commands\n\n" COLOR_RESET);
        return -1;
    }

    func(map,comm,type);
    return 0;
}
