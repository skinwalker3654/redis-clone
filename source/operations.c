#include "../header/operations.h"
#include "../header/color.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int op_set(HashMap **map, command_t *comm, execute_type type) {
    if(comm->counter != 3) {
        printf(COLOR_RED "\n(error) ERR wrong number of arguments for 'set' command\n" COLOR_RESET);
        printf(COLOR_YELLOW "(error) ERR correct usage: \"set KEY VALUE\"\n\n" COLOR_RESET);
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

    if(map_insert(*map,comm->argv[1],value,-1)==-1) {
        value_destroy(value);
        return -1;
    }

    if(type == EXECUTE_FROM_MAIN)
        printf(COLOR_GREEN "OK\n" COLOR_RESET);

    if(type == EXECUTE_FROM_MAIN) {
        FILE *file = fopen("AOF","a");
        if(!file) return 0;

        fprintf(file,"set %s %s\n",comm->argv[1],comm->argv[2]);
        fclose(file);
    }

    return 0;
}

int op_get(HashMap **map, command_t *comm, execute_type type) {
    if(comm->counter != 2) {
        printf(COLOR_RED "\n(error) ERR wrong number of arguments for 'get' command\n" COLOR_RESET);
        printf(COLOR_YELLOW "(error) ERR correct usage: \"get KEY\"\n\n" COLOR_RESET);
        return -1;
    }

    Value *value = map_get(*map,comm->argv[1]);
    if(!value) {
        printf(COLOR_DIM "(nil)\n" COLOR_RESET);
        return -1;
    }

    if(value->type == VAL_LIST) {
        printf(COLOR_RED "(error) ERR get command cannot be used on lists\n" COLOR_RESET);
        return -1;
    }

    switch(value->type) {
        case VAL_STRING:
            printf(COLOR_GREEN "\"%s\"\n" COLOR_RESET,value->string_value);
            break;
        case VAL_INTEGER:
            printf(COLOR_CYAN "\"%d\"\n" COLOR_RESET,value->integer_value);
            break;
        default:
            break;
    }

    return 0;
}

int op_del(HashMap **map, command_t *comm, execute_type type) {
    if(comm->counter < 2) {
        printf(COLOR_RED "\n(error) ERR wrong number of arguments for 'del' command\n" COLOR_RESET);
        printf(COLOR_YELLOW "(error) ERR correct usage: \"del KEY [KEY...]\"\n\n" COLOR_RESET);
        return -1;
    }

    int counter = 0;
    for(int i=1; i<comm->counter; i++) {
        if(map_remove(*map,comm->argv[i])==-1) continue;
        counter++;
    }

    if(type == EXECUTE_FROM_MAIN)
        printf(COLOR_GREEN "(integer) %d\n" COLOR_RESET,counter);

    if(type == EXECUTE_FROM_MAIN) {
        FILE *file = fopen("AOF","a");
        if(!file) return 0;

        fprintf(file,"del ");
        for(int i=1; i<comm->counter; i++) {
            fprintf(file,"%s ",comm->argv[i]);
        }

        fprintf(file,"\n");
        fclose(file);
    }

    return 0;
}

int op_exists(HashMap **map, command_t *comm, execute_type type) {
    if(comm->counter < 2) {
        printf(COLOR_RED "\n(error) ERR wrong number of arguments for 'exists' command\n" COLOR_RESET);
        printf(COLOR_YELLOW "(error) ERR correct usage: \"exists KEY [KEY...]\"\n\n" COLOR_RESET);
        return -1;
    }

    int counter = 0;
    for(int i=1; i<comm->counter; i++) {
        Value *value = map_get(*map,comm->argv[i]);
        if(!value) continue;
        counter++;
    }

    printf(COLOR_GREEN "(integer) %d\n" COLOR_RESET,counter);
    return 0;
}

int op_flushall(HashMap **map, command_t *comm, execute_type type) {
    if(comm->counter != 1) {
        printf(COLOR_RED "\n(error) ERR wrong number of arguments for 'flushall' command\n" COLOR_RESET);
        printf(COLOR_YELLOW "(error) ERR correct usage: \"flushall\" (No arguments needed)\n\n" COLOR_RESET);
        return -1;
    }

    if((*map)->keys == 0) {
        if(type == EXECUTE_FROM_MAIN)
            printf(COLOR_RED"(error) ERR map is empty\n"COLOR_RESET);
        return -1;
    }

    map_destroy(*map);
    *map = map_init();
    if(!map) return -1;

    if(type == EXECUTE_FROM_MAIN)
        printf(COLOR_GREEN "OK\n" COLOR_RESET);

    if(type == EXECUTE_FROM_MAIN) {
        FILE *file = fopen("AOF","a");
        if(!file) return 0;

        fprintf(file,"flushall\n");
        fclose(file);
    }

    return 0;
}

int op_clear(HashMap **map, command_t *comm, execute_type type) {
    if(comm->counter != 1) {
        printf(COLOR_RED "\n(error) ERR wrong number of arguments for 'clear' command\n" COLOR_RESET);
        printf(COLOR_YELLOW "(error) ERR correct usage: \"clear\" (No arguments needed)\n\n" COLOR_RESET);
        return -1;
    }

    system("clear");
    return 0;
}

int op_lrange(HashMap **map, command_t *comm, execute_type type) {
    if(comm->counter != 4) {
        printf(COLOR_RED "\n(error) ERR wrong number of arguments for 'lrange' command\n" COLOR_RESET);
        printf(COLOR_YELLOW "(error) ERR correct usage: \"lrange KEY START_POS {END_POS | -1}\"\n\n" COLOR_RESET);
        return -1;
    }

    Value *value = map_get(*map,comm->argv[1]);
    if(!value) {
        printf(COLOR_DIM "(empty array)\n" COLOR_RESET);
        return -1;
    }

    if(value->type != VAL_LIST) {
        printf(COLOR_RED "(error) ERR lrange command only works with lists\n" COLOR_RESET);
        return -1;
    }

    char *endPtr1;
    int start_pos = strtol(comm->argv[2],&endPtr1,10);
    if(*endPtr1 != '\0') {
        printf(COLOR_RED "(error) ERR invalid start position '%s'\n" COLOR_RESET,comm->argv[2]);
        return -1;
    }

    char *endPtr2;
    int end_pos = strtol(comm->argv[3],&endPtr2,10);
    if(*endPtr2 != '\0') {
        printf(COLOR_RED "(error) ERR invalid end position '%s'\n" COLOR_RESET,comm->argv[3]);
        return -1;
    }

    if(value->list->counter == 0) {
        printf(COLOR_RED"(error) ERR list is empty\n"COLOR_RESET);
        return -1;
    }

    if(end_pos < -1 || end_pos > value->list->counter-1) {
        printf(COLOR_RED "(error) ERR index out of bounds\n" COLOR_RESET);
        return -1;
    }

    if(start_pos < 0 || start_pos > value->list->counter-1) {
        printf(COLOR_RED "(error) ERR index out of bounds\n" COLOR_RESET);
        return -1;
    }

    printf("\n");
    for(int i=start_pos; i<=(end_pos == -1 ? value->list->counter -1 : end_pos); i++) {
        switch(value->list->items[i]->type) {
            case VAL_INTEGER:
                printf(COLOR_YELLOW "%d) " COLOR_RESET COLOR_CYAN "\"%d\"\n" COLOR_RESET, i+1, value->list->items[i]->integer_value);
                break;
            case VAL_STRING:
                printf(COLOR_YELLOW "%d) " COLOR_RESET COLOR_GREEN "\"%s\"\n" COLOR_RESET, i+1, value->list->items[i]->string_value);
                break;
            default:
                break;
        }
    }

    printf("\n");
    return 0;
}

int op_rpush(HashMap **map, command_t *comm, execute_type type) {
    if(comm->counter < 3) {
        printf(COLOR_RED "\n(error) ERR wrong number of arguments for 'rpush' command\n" COLOR_RESET);
        printf(COLOR_YELLOW "(error) ERR correct usage: \"rpush KEY ARG [ARG...]\"\n\n" COLOR_RESET);
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

        if(map_insert(*map,comm->argv[1],value_temp,-1)==-1) {
            value_destroy(value_temp);
            return -1;
        }

        if(type == EXECUTE_FROM_MAIN)
            printf(COLOR_GREEN "(integer) %d\n" COLOR_RESET, value_temp->list->counter);

        if(type == EXECUTE_FROM_MAIN) {
            FILE *file = fopen("AOF","a");
            if(!file) return 0;

            fprintf(file,"rpush %s ",comm->argv[1]);
            for(int i=2; i<comm->counter; i++) 
                fprintf(file,"%s ",comm->argv[i]);

            fprintf(file,"\n");
            fclose(file);
        }

        return 0;
    }

    if(value->type != VAL_LIST) {
        printf(COLOR_RED "(error) ERR rpush command only works with lists\n" COLOR_RESET);
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

    if(type == EXECUTE_FROM_MAIN)
        printf(COLOR_GREEN "(integer) %d\n" COLOR_RESET, value->list->counter);

    if(type == EXECUTE_FROM_MAIN) {
        FILE *file = fopen("AOF","a");
        if(!file) return 0;

        fprintf(file,"rpush %s ",comm->argv[1]);
        for(int i=2; i<comm->counter; i++) 
            fprintf(file,"%s ",comm->argv[i]);

        fprintf(file,"\n");
        fclose(file);
    }

    return 0;
}

int op_rpop(HashMap **map, command_t *comm, execute_type type) {
    if(comm->counter > 3) {
        printf(COLOR_RED "\n(error) ERR wrong number of arguments for 'rpop' command\n" COLOR_RESET);
        printf(COLOR_YELLOW "(error) ERR correct usage: \"rpop KEY [COUNT]\"\n\n" COLOR_RESET);
        return -1;
    }

    Value *value = map_get(*map,comm->argv[1]);
    if(!value) {
        if(type == EXECUTE_FROM_MAIN)
            printf(COLOR_DIM "(nil)\n" COLOR_RESET);
        return -1;
    }

    if(value->type != VAL_LIST) {
        printf(COLOR_RED "(error) ERR rpop command only works with lists\n" COLOR_RESET);
        return -1;
    }

    if(value->list->counter == 0) {
        printf(COLOR_RED "(error) ERR list is empty\n" COLOR_RESET);
        return -1;
    }

    if(comm->counter == 2) {
        switch(value->list->items[value->list->counter-1]->type) {
            case VAL_INTEGER:
                if(type == EXECUTE_FROM_MAIN)
                    printf(COLOR_CYAN "(integer) %d\n" COLOR_RESET, value->list->items[value->list->counter-1]->integer_value);
                break;
            case VAL_STRING:
                if(type == EXECUTE_FROM_MAIN)
                    printf(COLOR_GREEN "\"%s\"\n" COLOR_RESET, value->list->items[value->list->counter-1]->string_value);
                break;
            default:
                break;
        }

        value_destroy(value->list->items[value->list->counter-1]);
        value->list->counter--;

        if(type == EXECUTE_FROM_MAIN) {
            FILE *file = fopen("AOF","a");
            if(!file) return 0;

            fprintf(file,"rpop %s\n",comm->argv[1]);
            fclose(file);
        }

        return 0;
    } else {
        char *endPtr;
        int number = strtol(comm->argv[2],&endPtr,10);
        if(*endPtr != '\0') {
            printf(COLOR_RED"(error) ERR Invalid count '%s'\n"COLOR_RESET,comm->argv[2]);
            return -1;
        }

        if(number <= 0 || number > value->list->counter) {
            printf(COLOR_RED "(error) ERR index out of bounds\n" COLOR_RESET);
            return -1;
        }

        printf("\n");
        for(int i=0; i<number; i++) {
            int index = value->list->counter-1;
            switch(value->list->items[index]->type) {
                case VAL_INTEGER:
                    if(type == EXECUTE_FROM_MAIN)
                        printf(COLOR_CYAN "%d) \"%d\"\n" COLOR_RESET,i+1,value->list->items[index]->integer_value);
                    break;
                case VAL_STRING:
                    if(type == EXECUTE_FROM_MAIN)
                        printf(COLOR_GREEN "%d) \"%s\"\n" COLOR_RESET,i+1,value->list->items[index]->string_value);
                    break;
                default:
                    break;
            }

            value_destroy(value->list->items[index]);
            value->list->items[index] = NULL;
            value->list->counter--;
        }

        printf("\n");

        if(type == EXECUTE_FROM_MAIN) {
            FILE *file = fopen("AOF","a");
            if(!file) return 0;

            fprintf(file,"rpop %s %s\n",comm->argv[1],comm->argv[2]);
            fclose(file);
        }

        return 0;
    }

    return 0;
}

int op_lset(HashMap **map, command_t *comm, execute_type type) {
    if(comm->counter != 4) {
        printf(COLOR_RED "\n(error) ERR wrong number of arguments for 'lset' command\n" COLOR_RESET);
        printf(COLOR_YELLOW "(error) ERR correct usage: \"lset KEY INDEX VALUE\"\n\n" COLOR_RESET);
        return -1;
    }

    Value *value = map_get(*map,comm->argv[1]);
    if(!value) {
        if(type == EXECUTE_FROM_MAIN)
            printf(COLOR_RED "(error) ERR no such key\n" COLOR_RESET);
        return -1;
    }

    if(value->type != VAL_LIST) {
        printf(COLOR_RED "(error) ERR lset command only works with lists\n" COLOR_RESET);
        return -1;
    }

    char *endPtr1;
    int index = strtol(comm->argv[2],&endPtr1,10);
    if(*endPtr1 != '\0') {
        printf(COLOR_RED "(error) ERR invalid index '%s'\n" COLOR_RESET, comm->argv[2]);
        return -1;
    }

    if(index < 0 || index > value->list->counter-1) {
        printf(COLOR_RED "(error) ERR index out of bounds\n" COLOR_RESET);
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

    if(type == EXECUTE_FROM_MAIN)
        printf(COLOR_GREEN "OK\n" COLOR_RESET);

    if(type == EXECUTE_FROM_MAIN) {
        FILE *file = fopen("AOF","a");
        if(!file) return 0;

        fprintf(file,"lset %s %s %s\n",comm->argv[1],comm->argv[2],comm->argv[3]);
        fclose(file);
    }

    return 0;
}

int op_lindex(HashMap **map, command_t *comm, execute_type type) {
    if(comm->counter != 3) {
        printf(COLOR_RED "\n(error) ERR wrong number of arguments for 'lindex' command\n" COLOR_RESET);
        printf(COLOR_YELLOW "(error) ERR correct usage: \"lindex KEY INDEX\"\n\n" COLOR_RESET);
        return -1;
    }

    Value *value = map_get(*map, comm->argv[1]);
    if(!value) {
        printf(COLOR_DIM "(nil)\n" COLOR_RESET);
        return -1;
    }

    if(value->type != VAL_LIST) {
        printf(COLOR_RED "(error) ERR lindex command only works with lists\n" COLOR_RESET);
        return -1;
    }

    char *endPtr;
    int index = strtol(comm->argv[2], &endPtr, 10);
    if(*endPtr != '\0') {
        printf(COLOR_RED "(error) ERR invalid index '%s'\n" COLOR_RESET, comm->argv[2]);
        return -1;
    }

    if(index < 0 || index > value->list->counter-1) {
        printf(COLOR_RED "(error) ERR index out of range %d\n" COLOR_RESET, index);
        return -1;
    }

    Value *item = value->list->items[index];
    switch(item->type) {
        case VAL_STRING:
            printf(COLOR_GREEN "\"%s\"\n" COLOR_RESET, item->string_value);
            break;
        case VAL_INTEGER:
            printf(COLOR_CYAN "(integer) %d\n" COLOR_RESET, item->integer_value);
            break;
        default:
            break;
    }

    return 0;
}

int op_ttl(HashMap **map, command_t *comm, execute_type type) {
    if(comm->counter != 2) {
        printf(COLOR_RED "\n(error) ERR wrong number of arguments for 'ttl' command\n" COLOR_RESET);
        printf(COLOR_YELLOW "(error) ERR correct usage: \"ttl KEY\"\n\n" COLOR_RESET);
        return -1;
    }

    Value *value = map_get(*map,comm->argv[1]);
    if(!value) {
        printf(COLOR_DIM "(integer) -2\n" COLOR_RESET);
        return -1;
    }

    if(value->expire_at == -1) {
        printf(COLOR_YELLOW "(integer) -1\n" COLOR_RESET);
        return 0;
    }

    long long ttl = value->expire_at - time(NULL);
    if(ttl < 0) ttl = -1;
    
    if(ttl == -1) {
        printf(COLOR_YELLOW "(integer) -1\n" COLOR_RESET);
    } else if(ttl < 10) {
        printf(COLOR_RED "(integer) %lld\n" COLOR_RESET, ttl);
    } else if(ttl < 60) {
        printf(COLOR_YELLOW "(integer) %lld\n" COLOR_RESET, ttl);
    } else {
        printf(COLOR_GREEN "(integer) %lld\n" COLOR_RESET, ttl);
    }

    return 0;
}

int op_expire(HashMap **map, command_t *comm, execute_type type) {
    if(comm->counter != 3) {
        printf(COLOR_RED "\n(error) ERR wrong number of arguments for 'expire' command\n" COLOR_RESET);
        printf(COLOR_YELLOW "(error) ERR correct usage: \"expire KEY SECONDS\"\n\n" COLOR_RESET);
        return -1;
    }

    Value *value = map_get(*map,comm->argv[1]);
    if(!value) {
        if(type == EXECUTE_FROM_MAIN)
            printf(COLOR_GREEN "(integer) 0\n" COLOR_RESET);
        return -1;
    }

    char *endPtr;
    int seconds = strtol(comm->argv[2],&endPtr,10);
    if(*endPtr != '\0') {
        printf(COLOR_RED "(error) ERR invalid seconds '%s'\n" COLOR_RESET, comm->argv[2]);
        return -1;
    }

    value->expire_at = time(NULL) + seconds;
    if(type == EXECUTE_FROM_MAIN)
        printf(COLOR_GREEN "(integer) 1\n" COLOR_RESET);

    if(type == EXECUTE_FROM_MAIN) {
        FILE *file = fopen("AOF","a");
        if(!file) return 0;

        fprintf(file,"pexpireat %s %lld\n",comm->argv[1],value->expire_at);
        fclose(file);
    }

    return 0;
}


int op_keys(HashMap **map, command_t *comm, execute_type type) {
    if(comm->counter != 1) {
        printf(COLOR_RED "\n(error) ERR wrong number of arguments for 'keys' command\n" COLOR_RESET);
        printf(COLOR_YELLOW "(error) ERR correct usage: \"keys\" (No arguments needed)\n\n" COLOR_RESET);
        return -1;
    }

    if((*map)->keys == 0) {
        printf(COLOR_RED "(error) ERR database is empty\n" COLOR_RESET);
        return -1;
    }

    int counter = 1;
    printf("\n");
    for(int i=0; i<(*map)->size; i++) {
        Node *current = (*map)->buckets[i];
        while(current != NULL) {
            if(time(NULL) >= current->value->expire_at && current->value->expire_at != -1) {
                (*map)->keys--;
                current = current->next;
                continue;
            }
            printf(COLOR_YELLOW "%d) " COLOR_RESET COLOR_GREEN "\"%s\"\n" COLOR_RESET,counter,current->key);
            counter++;
            current = current->next;
        }
    }

    printf("\n");
    return 0;
}

int op_incrby(HashMap **map, command_t *comm, execute_type type) {
    if(comm->counter != 3) {
        printf(COLOR_RED "\n(error) ERR wrong number of arguments for 'incrby' command\n" COLOR_RESET);
        printf(COLOR_YELLOW "(error) ERR correct usage: \"incrby KEY INCREMENT\"\n\n" COLOR_RESET);
        return -1;
    }

    Value *value = map_get(*map,comm->argv[1]);
    if(!value) {
        if(type == EXECUTE_FROM_MAIN)
            printf(COLOR_DIM "(nil)\n" COLOR_RESET);
        return -1;
    }

    if(value->type != VAL_INTEGER) {
        printf(COLOR_RED "(error) ERR incrby command only works with integers\n" COLOR_RESET);
        return -1;
    }

    char *endPtr;
    int increment = strtol(comm->argv[2],&endPtr,10);
    if(*endPtr != '\0') {
        printf("(error) ERR invalid increment '%s'\n",comm->argv[2]);
        return -1;
    }

    value->integer_value += increment;
    if(type == EXECUTE_FROM_MAIN)
        printf(COLOR_CYAN "(integer) %d\n" COLOR_RESET,value->integer_value);

    if(type == EXECUTE_FROM_MAIN) {
        FILE *file = fopen("AOF","a");
        if(!file) return 0;

        fprintf(file,"incrby %s %s\n",comm->argv[1],comm->argv[2]);
        fclose(file);
    }

    return 0;
}

int op_decrby(HashMap **map, command_t *comm, execute_type type) {
    if(comm->counter != 3) {
        printf(COLOR_RED "\n(error) ERR wrong number of arguments for 'decrby' command\n" COLOR_RESET);
        printf(COLOR_YELLOW "(error) ERR correct usage: \"decrby KEY DECREMENT\"\n\n" COLOR_RESET);
        return -1;
    }

    Value *value = map_get(*map,comm->argv[1]);
    if(!value) {
        if(type == EXECUTE_FROM_MAIN)
            printf(COLOR_DIM "(nil)\n" COLOR_RESET);
        return -1;
    }

    if(value->type != VAL_INTEGER) {
        printf(COLOR_RED "(error) ERR decrby command only works with integers\n" COLOR_RESET);
        return -1;
    }

    char *endPtr;
    int increment = strtol(comm->argv[2],&endPtr,10);
    if(*endPtr != '\0') {
        printf("(error) ERR invalid decrement '%s'\n",comm->argv[2]);
        return -1;
    }

    value->integer_value -= increment;
    if(type == EXECUTE_FROM_MAIN)
        printf(COLOR_CYAN "(integer) %d\n" COLOR_RESET,value->integer_value);

    if(type == EXECUTE_FROM_MAIN) {
        FILE *file = fopen("AOF","a");
        if(!file) return 0;

        fprintf(file,"decrby %s %s\n",comm->argv[1],comm->argv[2]);
        fclose(file);
    }

    return 0;
}

int op_rename(HashMap **map, command_t *comm, execute_type type) {
    if(comm->counter != 3) {
        printf(COLOR_RED "\n(error) ERR wrong number of arguments for 'rename' command\n" COLOR_RESET);
        printf(COLOR_YELLOW "(error) ERR correct usage: \"rename KEY NEW_KEY\"\n\n" COLOR_RESET);
        return -1;
    }

    if(map_rename(*map,comm->argv[1],comm->argv[2])==-1) 
        return -1;

    if(type == EXECUTE_FROM_MAIN)
        printf(COLOR_GREEN "OK\n" COLOR_RESET);

    if(type == EXECUTE_FROM_MAIN) {
        FILE *file = fopen("AOF","a");
        if(!file) return 0;

        fprintf(file,"rename %s %s\n",comm->argv[1],comm->argv[2]);
        fclose(file);
    }

    return 0;
}

int op_setex(HashMap **map, command_t *comm, execute_type type) {
    if(comm->counter != 4) {
        printf(COLOR_RED "\n(error) ERR wrong number of arguments for 'setex' command\n" COLOR_RESET);
        printf(COLOR_YELLOW "(error) ERR correct usage: \"set KEY SECONDS VALUE\"\n\n" COLOR_RESET);
        return -1;
    }

    char *endPtr1;
    long long seconds = strtol(comm->argv[2],&endPtr1,10);
    if(*endPtr1 != '\0') {
        printf("(error) ERR Invalid seconds '%s'\n",comm->argv[3]);
        return -1;
    }

    int is_number = 0;

    char *endPtr2;
    int number = strtol(comm->argv[3],&endPtr2,10);
    if(*endPtr2 == '\0')
        is_number = 1;

    Value *value = is_number ? value_create_integer(number) : value_create_string(comm->argv[3]);
    if(!value)
        return -1;

    if(map_insert(*map,comm->argv[1],value,seconds)==-1) {
        value_destroy(value);
        return -1;
    }

    if(type == EXECUTE_FROM_MAIN)
        printf(COLOR_GREEN "OK\n" COLOR_RESET);

    int index = hash_function(comm->argv[1]);
    if(type == EXECUTE_FROM_MAIN) {
        FILE *file = fopen("AOF","a");
        if(!file) return 0;

        fprintf(file,"set %s %s\n",comm->argv[1],comm->argv[3]);
        fprintf(file,"pexpireat %s %lld\n",comm->argv[1],(*map)->buckets[index]->value->expire_at);
        fclose(file);
    }

    return 0;

}

int op_pexpireat(HashMap **map, command_t *comm, execute_type type) {
    if(comm->counter != 3) {
        printf(COLOR_RED "\n(error) ERR wrong number of arguments for 'pexpireat' command\n" COLOR_RESET);
        printf(COLOR_YELLOW "(error) ERR correct usage: \"pexpireat KEY UNIX_TIME\"\n\n" COLOR_RESET);
        return -1;
    }

    Value *value = map_get(*map,comm->argv[1]);
    if(!value) {
        if(type == EXECUTE_FROM_MAIN)
            printf(COLOR_GREEN "(integer) 0\n" COLOR_RESET);
        return -1;
    }

    char *endPtr;
    long long expire_time = strtol(comm->argv[2],&endPtr,10);
    if(*endPtr != '\0') {
        printf(COLOR_RED "(error) ERR invalid unix_time '%s'\n" COLOR_RESET,comm->argv[2]);
        return -1;
    }
    
    value->expire_at = expire_time;
    if(type == EXECUTE_FROM_MAIN)
        printf(COLOR_GREEN "(integer) 1\n" COLOR_RESET);

    if(type == EXECUTE_FROM_MAIN) {
        FILE *file = fopen("AOF","a");
        if(!file) return 0;

        fprintf(file,"pexpireat %s %s\n",comm->argv[1],comm->argv[2]);
        fclose(file);
    }

    return 0;
}

int op_type(HashMap **map, command_t *comm, execute_type type) {
    if(comm->counter != 2) {
        printf(COLOR_RED "\n(error) ERR wrong number of arguments for 'type' command\n" COLOR_RESET);
        printf(COLOR_YELLOW "(error) ERR correct usage: \"type KEY\"\n\n" COLOR_RESET);
        return -1;
    }

    Value *value = map_get(*map,comm->argv[1]);
    if(!value) {
        printf(COLOR_DIM"none\n"COLOR_RESET);
        return -1;
    }
    
    switch(value->type) {
        case VAL_STRING:
            printf(COLOR_GREEN"string\n"COLOR_RESET);
            break;
        case VAL_INTEGER:
            printf(COLOR_CYAN"integer\n"COLOR_RESET);
            break;
        case VAL_LIST:
            printf(COLOR_WHITE"list\n"COLOR_RESET);
            break;
    }

    return 0;
}

int op_help(HashMap **map, command_t *comm, execute_type type) {
    if(comm->counter != 1) {
        printf(COLOR_RED "\n(error) ERR wrong number of arguments for 'help' command\n" COLOR_RESET);
        printf(COLOR_YELLOW "(error) ERR correct usage: \"help\" (No arguments needed)\n\n" COLOR_RESET);
        return -1;
    }

    printf(COLOR_BOLD COLOR_CYAN "\n╔════════════════════════════════════════════════════════════╗\n" COLOR_RESET);
    printf(COLOR_BOLD COLOR_CYAN "║" COLOR_RESET COLOR_BOLD COLOR_WHITE "                     Available Commands                     " COLOR_RESET COLOR_BOLD COLOR_CYAN "║\n" COLOR_RESET);
    printf(COLOR_BOLD COLOR_CYAN "╚════════════════════════════════════════════════════════════╝\n\n" COLOR_RESET);

    printf(COLOR_BOLD COLOR_YELLOW "  Value Commands:\n" COLOR_RESET);
    printf(COLOR_GREEN "    set" COLOR_RESET " key value               " COLOR_DIM "Create or update a key\n" COLOR_RESET);
    printf(COLOR_GREEN "    rename" COLOR_RESET " key new_key          " COLOR_DIM "Change key name\n" COLOR_RESET);
    printf(COLOR_GREEN "    incrby" COLOR_RESET " key increment        " COLOR_DIM "Increase key value by increment\n" COLOR_RESET);
    printf(COLOR_GREEN "    decrby" COLOR_RESET " key decrement        " COLOR_DIM "Decrease key value by decrement\n" COLOR_RESET);
    printf(COLOR_GREEN "    get" COLOR_RESET " key                     " COLOR_DIM "Get value of a key\n" COLOR_RESET);
    printf(COLOR_GREEN "    type" COLOR_RESET " key                    " COLOR_DIM "Get type of a key\n" COLOR_RESET);
    printf(COLOR_GREEN "    del" COLOR_RESET " key [key...]            " COLOR_DIM "Delete a key or many keys\n" COLOR_RESET);
    printf(COLOR_GREEN "    exists" COLOR_RESET " key [key..]          " COLOR_DIM "Check if key or keys exist\n" COLOR_RESET);
    printf(COLOR_GREEN "    flushall" COLOR_RESET "                    " COLOR_DIM "Delete all keys\n" COLOR_RESET);
    printf(COLOR_GREEN "    keys" COLOR_RESET "                        " COLOR_DIM "Get all keys\n" COLOR_RESET);

    printf("\n");
    printf(COLOR_BOLD COLOR_MAGENTA "  Expiration Commands:\n" COLOR_RESET);
    printf(COLOR_GREEN "    ttl" COLOR_RESET " key                     " COLOR_DIM "Show remaining lifetime\n" COLOR_RESET);
    printf(COLOR_GREEN "    expire" COLOR_RESET " key seconds          " COLOR_DIM "Set key expiration time to (now + seconds)\n" COLOR_RESET);
    printf(COLOR_GREEN "    pexpireat" COLOR_RESET " key unix_time     " COLOR_DIM "Set key expiration time to unix_time\n" COLOR_RESET);
    printf(COLOR_GREEN "    setex" COLOR_RESET " key seconds value     " COLOR_DIM "Create and Set key expiration time\n" COLOR_RESET);

    printf("\n");
    printf(COLOR_BOLD COLOR_CYAN "  List Commands:\n" COLOR_RESET);
    printf(COLOR_GREEN "    rpush" COLOR_RESET " key value [value...]  " COLOR_DIM "Append value to list or append values to list\n" COLOR_RESET);
    printf(COLOR_GREEN "    lrange" COLOR_RESET " key start end        " COLOR_DIM "Get values in range (-1 = all)\n" COLOR_RESET);
    printf(COLOR_GREEN "    rpop" COLOR_RESET " key [count]            " COLOR_DIM "Remove last value from list or remove last values\n" COLOR_RESET);
    printf(COLOR_GREEN "    lset" COLOR_RESET " key index value        " COLOR_DIM "Change value at index\n" COLOR_RESET);
    printf(COLOR_GREEN "    lindex" COLOR_RESET " key index            " COLOR_DIM "Get value at index\n" COLOR_RESET);

    printf("\n");
    printf(COLOR_BOLD COLOR_WHITE "  Utility Commands:\n" COLOR_RESET);
    printf(COLOR_GREEN "    clear" COLOR_RESET "                       " COLOR_DIM "Clear terminal\n" COLOR_RESET);
    printf(COLOR_GREEN "    help" COLOR_RESET "                        " COLOR_DIM "Show this help menu\n" COLOR_RESET);
    printf("\n");

    return 0;
}
