#ifndef PARSER_H
#define PARSER_H

#define INIT_CAP 2
typedef struct {
    char **argv;
    int capacity;
    int counter;
} command_t;

/* owner-ship */
command_t *parser_command_init();
void parser_command_destroy(command_t *comm);

/* get next argument and pushing function */
char *parser_get_next_arg(char *input, int *pos);
int parser_command_push(command_t *comm, char *arg);
int parser_parse_command(command_t *comm, char *text, int pos);

#endif
