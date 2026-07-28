#include "../header/parser.h"
#include "../header/color.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

command_t *parser_command_init() {
    command_t *new_comm = malloc(sizeof(command_t));
    if(!new_comm) {
        printf(COLOR_RED "(error) ERR Failed to allocate memory for the command\n" COLOR_RESET);
        return NULL;
    }

    new_comm->argv = malloc(sizeof(char*)*INIT_CAP);
    if(!new_comm->argv) {
        printf(COLOR_RED "(error) ERR Failed to allocate memory for the command's arguments list\n" COLOR_RESET);
        free(new_comm);
        return NULL;
    }

    new_comm->counter = 0;
    new_comm->capacity = INIT_CAP;

    return new_comm;
}

void parser_command_destroy(command_t *comm) {
    if(!comm) return;

    for(int i=0; i<comm->counter; i++)
        free(comm->argv[i]);

    free(comm->argv);
    free(comm);
}

static void string_push_char(char **string, char ch) {
    if(!*string) {
        *string = malloc(2);
        if(!*string) return;

        (*string)[0] = ch;
        (*string)[1] = '\0';
        return;
    }

    int size = strlen(*string);
    char *temp = realloc(*string,size+2);
    if(!temp) return;

    *string = temp;
    (*string)[size] = ch;
    (*string)[size+1] = '\0';

    return;
}

char *parser_get_next_arg(char *input, int *pos) {
    while(isspace(input[*pos])) (*pos)++;
    if(input[*pos] == '\0') {
        char *buffer = malloc(1);
        if(!buffer) {
            printf(COLOR_RED "(error) ERR Failed to parse null terminator\n" COLOR_RESET);
            return NULL;
        }

        *buffer = '\0';
        return buffer;
    }

    if(input[*pos] == '"') {
        char *buffer = NULL;

        (*pos)++;
        while(input[*pos] != '"' && input[*pos] != '\0')
            string_push_char(&buffer,input[(*pos)++]);

        if(input[*pos] != '"') {
            printf(COLOR_RED "(error) ERR String has never been closed properly\n" COLOR_RESET);
            free(buffer);
            return NULL;
        }

        (*pos)++;
        return buffer;
    }

    char *buffer = NULL;
    while(input[*pos] != '\0' && !isspace(input[*pos]))
        string_push_char(&buffer,input[(*pos)++]);

    return buffer;
}

int parser_command_push(command_t *comm, char *arg) {
    if(comm->counter >= comm->capacity) {
        int temp_cap = comm->capacity * 2;
        char **temp = realloc(comm->argv,sizeof(char*)*temp_cap);
        if(!temp) {
            printf(COLOR_RED "(error) ERR Failed to allocate memory for more arguments\n" COLOR_RESET);
            return -1;
        }

        comm->argv = temp;
        comm->capacity = temp_cap;
    }

    comm->argv[comm->counter] = strdup(arg);
    if(!comm->argv[comm->counter]) {
        printf(COLOR_RED "(error) ERR Failed to allocate memory for the argument\n" COLOR_RESET);
        return -1;
    }

    comm->counter++;
    return 0;
}

int parser_parse_command(command_t *comm, char *input, int pos) {
    while(1) {
        char *arg = parser_get_next_arg(input,&pos);
        if(!arg) return -1;

        if(*arg == '\0') {
            free(arg);
            return 0;
        }

        parser_command_push(comm,arg);
        free(arg);
    }
}
