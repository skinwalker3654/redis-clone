#include "../header/hashmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

List *list_init() {
    List *list_new = malloc(sizeof(List));
    if(!list_new) {
        printf("error from list_init: Failed to allocate memory for the list\n");
        return NULL;
    }

    list_new->items = malloc(sizeof(Value*)*INIT_CAP);
    if(!list_new->items) {
        printf("error from list_init: Failed to allocate memory for the items in the list\n");
        free(list_new);
        return NULL;
    }

    list_new->capacity = INIT_CAP;
    list_new->counter = 0;

    return list_new;
}

void list_destroy(List *list) {
    if(!list) return;

    for(int i=0; i<list->counter; i++)
        value_destroy(list->items[i]);

    free(list->items);
    free(list);
}

int list_append(List *list, Value *value) {
    if(!value || !list) return -1;
    if(value->type == VAL_LIST) {
        printf("error from list_append: Cannot append a list inside an other list\n");
        return -1;
    }

    if(list->counter >= list->capacity) {
        int temp_cap = list->capacity * 2;
        Value **temp = realloc(list->items,sizeof(Value*)*temp_cap);
        if(!temp) {
            printf("error from list_append: Failed to allocate memory for more values\n");
            return -1;
        }

        list->items = temp;
        list->capacity = temp_cap;
    }

    list->items[list->counter] = value;
    list->counter++;

    return 0;
}

Value *value_create_string(char *string) {
    Value *new_value = malloc(sizeof(Value));
    if(!new_value) {
        printf("error from value_create_string: Failed to allocate memory for the value\n");
        return NULL;
    }

    new_value->type = VAL_STRING; 
    new_value->string_value = strdup(string);
    if(!new_value->string_value) {
        printf("error from value_create_string: Failed to allocate memory for the string_value\n");
        free(new_value);
        return NULL;
    }

    return new_value;
}

Value *value_create_integer(int integer) {
    Value *new_value = malloc(sizeof(Value));
    if(!new_value) {
        printf("error from value_create_string: Failed to allocate memory for the value\n");
        return NULL;
    }

    new_value->type = VAL_INTEGER; 
    new_value->integer_value = integer;

    return new_value;
}

Value *value_create_list(List *list) {
    Value *new_value = malloc(sizeof(Value));
    if(!new_value) {
        printf("error from value_create_string: Failed to allocate memory for the value\n");
        return NULL;
    }

    new_value->type = VAL_LIST; 
    new_value->list = list;

    return new_value;
}

void value_destroy(Value *value) {
    if(!value) return;

    switch(value->type) {
        case VAL_INTEGER:
            free(value);
            break;
        case VAL_STRING:
            free(value->string_value);
            free(value);
            break;
        case VAL_LIST:
            list_destroy(value->list);
            free(value);
            break;
    }
}


HashMap *map_init() {
    HashMap *new_map = malloc(sizeof(HashMap));
    if(!new_map) {
        printf("Failed to allocate memory for the hash_map\n");
        return NULL;
    }

    new_map->buckets = calloc(HASH_MAP_SIZE,sizeof(Node*));
    if(!new_map->buckets) {
        printf("Failed to allocate memory for the hash_map's buckets\n");
        free(new_map);
        return NULL;
    }

    new_map->size = HASH_MAP_SIZE;
    return new_map;
}

void map_destroy(HashMap *map) {
    if(!map) return;

    for(int i=0; i<map->size; i++) {
        Node *current = map->buckets[i];
        while(current != NULL) {
            Node *temp = current;
            current = current->next;
            free(temp->key);
            value_destroy(temp->value);
            free(temp);
        }
    }

    free(map->buckets);
    free(map);
}

/* hash function */
unsigned int hash(char *key) {
    unsigned int hash = 5381;
    int c;

    while((c = *key++))
        hash = ((hash << 5) + hash) + c;

    return hash % HASH_MAP_SIZE;
}

/* methods */
int map_insert(HashMap *map, char *key, Value *value) {
    int index = hash(key);
    Node *current = map->buckets[index];

    /* insert values in a chain for colisions */
    while(current != NULL) {
        if(strcmp(current->key,key)==0) {
            value_destroy(current->value);
            current->value = value;
            return 0;
        }
        current = current->next;
    }

    /* initializing new node */
    Node *new_node = malloc(sizeof(Node));
    if(!new_node) {
        printf("error from map_insert: Failed to allocate memory for the new node\n");
        return -1;
    }

    new_node->key = strdup(key);
    if(!new_node->key) {
        printf("error from map_insert: Failed to allocate memory for the new key\n");
        free(new_node);
        return -1;
    }

    new_node->value = value;
    new_node->value->expire_at = -1;

    /* inserting the new node */
    new_node->next = map->buckets[index];
    map->buckets[index] = new_node;

    return 0;
}

Value *map_get(HashMap *map, char *key) {
    int index = hash(key);
    Node *current = map->buckets[index];

    while(current != NULL) {
        if(strcmp(current->key,key)==0) {
            if(current->value->expire_at != -1) {
                if(time(NULL) >= current->value->expire_at) {
                    map_remove(map,current->key);
                    return NULL;
                }
            }
            return current->value;
        }
        current = current->next;
    }

    return NULL;
}

int map_remove(HashMap *map, char *key) {
    int index = hash(key);
    Node *current = map->buckets[index];
    Node *prev = NULL;

    /* find and delete value */
    while(current != NULL) {
        if(strcmp(current->key,key)==0) {
            if(prev != NULL) {
                prev->next = current->next;
            } else {
                map->buckets[index] = current->next;
            }

            free(current->key);
            value_destroy(current->value);
            free(current);
            return 0;
        }

        prev = current;
        current = current->next;
    }

    printf("(nil)\n");
    return -1;
}
