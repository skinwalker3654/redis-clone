#ifndef OPERATIONS_H
#define OPERATIONS_H

#include "parser.h"
#include "hashmap.h"

int op_set(HashMap **map, command_t *comm);
int op_get(HashMap **map, command_t *comm);
int op_del(HashMap **map, command_t *comm);
int op_exists(HashMap **map, command_t *comm);
int op_flushall(HashMap **map, command_t *comm);
int op_clear(HashMap **map, command_t *comm);
int op_ttl(HashMap **map, command_t *comm);
int op_expire(HashMap **map, command_t *comm);
int op_lrange(HashMap **map, command_t *comm);
int op_rpush(HashMap **map, command_t *comm);
int op_rpop(HashMap **map, command_t *comm);
int op_lset(HashMap **map, command_t *comm);
int op_lindex(HashMap **map, command_t *comm);
int op_help(HashMap **map, command_t *comm);

#endif
