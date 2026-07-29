#ifndef OPERATIONS_H
#define OPERATIONS_H

#include "parser.h"
#include "hashmap.h"

typedef enum {
    EXECUTE_FROM_MAIN,
    EXECUTE_FROM_AOF,
} execute_type;

int op_set(HashMap **map, command_t *comm, execute_type type);
int op_get(HashMap **map, command_t *comm, execute_type type);
int op_del(HashMap **map, command_t *comm, execute_type type);
int op_exists(HashMap **map, command_t *comm, execute_type type);
int op_flushall(HashMap **map, command_t *comm, execute_type type);
int op_clear(HashMap **map, command_t *comm, execute_type type);
int op_ttl(HashMap **map, command_t *comm, execute_type type);
int op_expire(HashMap **map, command_t *comm, execute_type type);
int op_lrange(HashMap **map, command_t *comm, execute_type type);
int op_rpush(HashMap **map, command_t *comm, execute_type type);
int op_rpop(HashMap **map, command_t *comm, execute_type type);
int op_lset(HashMap **map, command_t *comm, execute_type type);
int op_lindex(HashMap **map, command_t *comm, execute_type type);
int op_keys(HashMap **map, command_t *comm, execute_type type);
int op_incrby(HashMap **map, command_t *comm, execute_type type);
int op_decrby(HashMap **map, command_t *comm, execute_type type);
int op_rename(HashMap **map, command_t *comm, execute_type type);
int op_setex(HashMap **map, command_t *comm, execute_type type);
int op_pexpireat(HashMap **map, command_t *comm, execute_type type);
int op_help(HashMap **map, command_t *comm, execute_type type);

#endif
