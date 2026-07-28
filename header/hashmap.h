#ifndef HASH_MAP_H
#define HASH_MAP_H

#define HASH_MAP_SIZE 100
#define INIT_CAP 2

typedef enum {
    VAL_STRING,
    VAL_INTEGER,
    VAL_LIST,
} ValueType;

typedef struct Value Value;

typedef struct {
    Value **items;
    int counter;
    int capacity;
} List;

/* list operations */
List *list_init();
void list_destroy(List *list);
int list_append(List *list, Value *value);

typedef struct Value {
    ValueType type;
    long long expire_at;
    union {
        char *string_value;
        int integer_value;
        List *list;
    };
} Value;

/* value operations */
Value *value_create_string(char *string);
Value *value_create_integer(int integer);
Value *value_create_list(List *list);
void value_destroy(Value *value);

typedef struct Node {
    char *key;
    Value *value;
    struct Node *next;
} Node;

typedef struct {
    Node **buckets;
    int keys;
    int size;
} HashMap;

/* hashmap operations */
int hash_function();
HashMap *map_init();
void map_destroy(HashMap *map);

int map_insert(HashMap *map, char *key, Value *value);
int map_remove(HashMap *map, char *key);
int map_rename(HashMap *map, char *key, char *new_key);
Value *map_get(HashMap *map, char *key);

#endif
