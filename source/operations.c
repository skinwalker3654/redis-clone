#include "../header/operations.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int op_set(HashMap **map, command_t *comm) {
    if(comm->counter != 3) {
        printf("invalid usage of command set\n");
        printf("correct usage: \"set KEY VALUE\"\n");
        return -1;
    }

    int is_number = 0;

    char *endPtr;
    int number = strtol(comm->argv[2],&endPtr,10);
    if(*endPtr == '\0')
        is_number = 1;

    Value *value = is_number ? value_create_integer(number) : value_create_string(comm->argv[2]);
    if(!value)
        return -1;

    if(map_insert(*map,comm->argv[1],value)==-1) {
        value_destroy(value);
        return -1;
    }

    return 0;
}

int op_get(HashMap **map, command_t *comm) {
    if(comm->counter != 2) {
        printf("invalid usage of command get\n");
        printf("correct usage: \"get KEY\"\n");
        return -1;
    }

    Value *value = map_get(*map,comm->argv[1]);
    if(!value) {
        printf("error key '%s' was not found in the list\n",comm->argv[1]);
        return -1;
    }

    if(value->type == VAL_LIST) {
        printf("error you cannot get a list with the get command\n");
        return -1;
    }

    switch(value->type) {
        case VAL_STRING:
            printf("\"%s\"\n",value->string_value);
            break;
        case VAL_INTEGER:
            printf("\"%d\"\n",value->integer_value);
            break;
        default:
            break;
    }

    return 0;
}

int op_del(HashMap **map, command_t *comm) {
    if(comm->counter != 2) {
        printf("invalid usage of command del\n");
        printf("correct usage: \"del KEY\"\n");
        return -1;
    }

    if(map_remove(*map,comm->argv[1])==-1) 
        return -1;

    return 0;
}

int op_exists(HashMap **map, command_t *comm) {
    if(comm->counter != 2) {
        printf("invalid usage of command exists\n");
        printf("correct usage: \"exists KEY\"\n");
        return -1;
    }

    Value *value = map_get(*map,comm->argv[1]);
    if(!value) {
        printf("exists_status: 0\n");
        return 0;
    }

    printf("exists_status: 1\n");
    return 0;
}

int op_flushall(HashMap **map, command_t *comm) {
    if(comm->counter != 1) {
        printf("invalid usage of command flushall\n");
        printf("correct usage: \"flushall\" (No arguments needed)\n");
        return -1;
    }

    map_destroy(*map);
    *map = map_init();
    if(!map) return -1;

    return 0;
}

int op_clear(HashMap **map, command_t *comm) {
    if(comm->counter != 1) {
        printf("invalid usage of command clear\n");
        printf("correct usage: \"clear\" (No arguments needed)\n");
        return -1;
    }

    system("clear");
    return 0;
}

int op_lrange(HashMap **map, command_t *comm) {
    if(comm->counter != 4) {
        printf("invalid usage of command lrange\n");
        printf("correct usage: \"lrange KEY START_POS {END_POS | -1}\"\n");
        return -1;
    }

    Value *value = map_get(*map,comm->argv[1]);
    if(!value) {
        printf("error key '%s' was not found in the list\n",comm->argv[1]);
        return -1;
    }

    if(value->type != VAL_LIST) {
        printf("error lrange command only works on lists\n");
        return -1;
    }

    char *endPtr1;
    int start_pos = strtol(comm->argv[2],&endPtr1,10);
    if(*endPtr1 != '\0') {
        printf("error you have put an invalid value on start pos '%s'\n",comm->argv[2]);
        return -1;
    }
    
    char *endPtr2;
    int end_pos = strtol(comm->argv[3],&endPtr2,10);
    if(*endPtr2 != '\0') {
        printf("error you have put an invalid value on end pos '%s'\n",comm->argv[3]);
        return -1;
    }

    if(end_pos < -1 || end_pos > value->list->counter-1) {
        printf("error index is out of bounds\n");
        return -1;
    }

    if(start_pos < 0 || start_pos > value->list->counter-1) {
        printf("error index is out of bounds\n");
        return -1;
    }
    
    for(int i=start_pos; i<=(end_pos == -1 ? value->list->counter -1 : end_pos); i++) {
        switch(value->list->items[i]->type) {
            case VAL_INTEGER:
                printf("%d) \"%d\"\n",i+1,value->list->items[i]->integer_value);
                break;
            case VAL_STRING:
                printf("%d) \"%s\"\n",i+1,value->list->items[i]->string_value);
                break;
            default:
                break;
        }
    }

    return 0;
}

int op_rpush(HashMap **map, command_t *comm) {
    if(comm->counter < 3) {
        printf("invalid usage of command rpush\n");
        printf("correct usage: \"rpush KEY arg [arg...]\"\n");
        return -1;
    }

    Value *value = map_get(*map,comm->argv[1]);
    if(!value) {
        List *list = list_init();
        if(!list) return -1;

        /* appending the values into the new list */
        for(int i=2; i<comm->counter; i++) {
            int is_number = 0;

            char *endPtr;
            int number = strtol(comm->argv[i],&endPtr,10);
            if(*endPtr == '\0') is_number = 1;

            if(is_number) {
                Value *value_temp = value_create_integer(number);
                if(!value_temp) {
                    list_destroy(list);
                    return -1;
                }
                list_append(list,value_temp);
                continue;
            } else {
                Value *value_temp = value_create_string(comm->argv[i]);
                if(!value_temp) {
                    list_destroy(list);
                    return -1;
                }
                list_append(list,value_temp);
                continue;
            }
        }

        Value *value_temp = value_create_list(list);
        if(!value_temp) {
            list_destroy(list);
            return -1;
        }

        if(map_insert(*map,comm->argv[1],value_temp)==-1) {
            value_destroy(value_temp);
            return -1;
        }

        return 0;
    }

    if(value->type != VAL_LIST) {
        printf("error rpush command only works on lists\n");
        return -1;
    }

    for(int i=2; i<comm->counter; i++) {
        int is_number = 0;

        char *endPtr;
        int number = strtol(comm->argv[i],&endPtr,10);
        if(*endPtr == '\0') is_number = 1;

        if(is_number) {
            Value *value_temp = value_create_integer(number);
            if(!value_temp) return -1;

            list_append(value->list,value_temp);
            continue;
        } else {
            Value *value_temp = value_create_string(comm->argv[i]);
            if(!value_temp) return -1;
            list_append(value->list,value_temp);
            continue;
        }
    }

    return 0;
}

int op_rpop(HashMap **map, command_t *comm) {
    if(comm->counter != 2) {
        printf("invalid usage of command rpop\n");
        printf("correct usage: \"rpop KEY\"\n");
        return -1;
    }

    Value *value = map_get(*map,comm->argv[1]);
    if(!value) {
        printf("error key '%s' was not found in the list\n",comm->argv[1]);
        return -1;
    }

    if(value->type != VAL_LIST) {
        printf("error rpop command only works on lists\n");
        return -1;
    }

    switch(value->list->items[value->list->counter-1]->type) {
        case VAL_INTEGER:
            printf("\"%d\"\n",value->list->items[value->list->counter-1]->integer_value);
            break;
        case VAL_STRING:
            printf("\"%s\"\n",value->list->items[value->list->counter-1]->string_value);
            break;
        default:
            break;
    }

    value_destroy(value->list->items[value->list->counter-1]);
    value->list->counter--;

    return 0;
}

int op_lset(HashMap **map, command_t *comm) {
    if(comm->counter != 4) {
        printf("invalid usage of command lset\n");
        printf("correct usage: \"lset KEY INDEX VALUE\"\n");
        return -1;
    }

    Value *value = map_get(*map,comm->argv[1]);
    if(!value) {
        printf("error key '%s' was not found in the list\n",comm->argv[1]);
        return -1;
    }

    if(value->type != VAL_LIST) {
        printf("error lset command only works on lists\n");
        return -1;
    }

    char *endPtr1;
    int index = strtol(comm->argv[2],&endPtr1,10);
    if(*endPtr1 != '\0') {
        printf("error invalid index '%s'\n",comm->argv[2]);
        return -1;
    }

    if(index < 0 || index > value->list->counter-1) {
        printf("error index is out of bounds\n");
        return -1;
    }

    int is_number = 0;

    char *endPtr2;
    int number = strtol(comm->argv[3],&endPtr2,10);
    if(*endPtr2 == '\0') is_number = 1;

    Value *value_temp;
    if(is_number) {
        value_temp = value_create_integer(number);
        if(!value_temp) return -1;
    } else {
        value_temp = value_create_string(comm->argv[3]);
        if(!value_temp) return -1;
    }

    value_destroy(value->list->items[index]);
    value->list->items[index] = value_temp;

    return 0;
}

int op_lindex(HashMap **map, command_t *comm) {
    if(comm->counter != 3) {
        printf("invalid usage of command lindex\n");
        printf("correct usage: \"lindex KEY INDEX\"\n");
        return -1;
    }

    Value *value = map_get(*map, comm->argv[1]);
    if(!value) return -1;

    if(value->type != VAL_LIST) {
        printf("error lindex command only works on lists\n");
        return -1;
    }

    char *endPtr;
    int index = strtol(comm->argv[2], &endPtr, 10);
    if(*endPtr != '\0') {
        printf("invalid index '%s'\n",comm->argv[2]);
        return -1;
    }

    if(index < 0 || index > value->list->counter-1) {
        printf("index out of range %d\n",index);
        return -1;
    }

    Value *item = value->list->items[index];
    switch(item->type) {
        case VAL_STRING:
            printf("\"%s\"\n", item->string_value);
            break;
        case VAL_INTEGER:
            printf("%d\n", item->integer_value);
            break;
        default:
            break;
    }

    return 0;
}

int op_ttl(HashMap **map, command_t *comm) {
    if(comm->counter != 2) {
        printf("invalid usage of command ttl\n");
        printf("correct usage: \"ttl KEY\"\n");
        return -1;
    }

    Value *value = map_get(*map,comm->argv[1]);
    if(!value) {
        printf("error key '%s' does not found in the list\n",comm->argv[1]);
        return -1;
    }

    printf("time remaining: %llds\n",value->expire_at - time(NULL));
    return 0;
}

int op_expire(HashMap **map, command_t *comm) {
    if(comm->counter != 3) {
        printf("invalid usage of command expire\n");
        printf("correct usage: \"expire KEY SECONDS\"\n");
        return -1;
    }

    Value *value = map_get(*map,comm->argv[1]);
    if(!value) {
        printf("error key '%s' does not found in the list\n",comm->argv[1]);
        return -1;
    }

    char *endPtr;
    int seconds = strtol(comm->argv[2],&endPtr,10);
    if(*endPtr != '\0') {
        printf("error invalid seconds '%s'\n",comm->argv[2]);
        return -1;
    }

    value->expire_at = time(NULL) + seconds;
    return 0;
}

int op_help(HashMap **map, command_t *comm) {
    if(comm->counter != 1) {
        printf("invalid usage of command help\n");
        printf("correct usage: \"help\" (No arguments needed)\n");
        return -1;
    }
    
    printf("Commands:\n");
    printf("\n");

    printf("  String commands:\n");
    printf("    set key value               Create or update a key\n");
    printf("    get key                     Get value of a key\n");
    printf("    del key                     Delete a key\n");
    printf("    exists key                  Check if key exists\n");
    printf("    flushall                    Delete all keys\n");

    printf("\n");
    printf("  Expiration commands:\n");
    printf("    ttl key                     Show remaining lifetime\n");
    printf("    expire key seconds          Set key expiration time\n");

    printf("\n");
    printf("  List commands:\n");
    printf("    rpush key value [value...]  Append value to list\n");
    printf("    lrange key start end        Get values in range (-1 = all)\n");
    printf("    rpop key                    Remove last value from list\n");
    printf("    lset key index value        Change value at index\n");
    printf("    lindex key index            Get value at index\n");

    printf("\n");
    printf("  Utility commands:\n");
    printf("    clear                       Clear terminal\n");
    printf("    help                        Show this help menu\n");
    printf("\n");

    return 0;
}
