#include <stdio.h>
#include <string.h>
#include "../header/commands.h"
#include "../header/color.h"

#define BUFF_SIZE 256
static int source_pos = 0;

void load_aof(HashMap **map) {
    FILE *file = fopen("AOF","r");
    if(!file) return;

    char command[BUFF_SIZE];
    while(fgets(command,sizeof(command),file)) {
        command[strcspn(command,"\n")] = '\0';
        command_t *comm = parser_command_init(); 
        if(!comm) break;

        source_pos = 0;
        if(parser_parse_command(comm,command,source_pos)==-1) {
            parser_command_destroy(comm);
            break;
        }

        if(execute(map,comm,EXECUTE_FROM_AOF)==-1) {
            parser_command_destroy(comm);
            break;
        }

        parser_command_destroy(comm);
    }

    fclose(file);
}

int main(void) {
    FILE *file = fopen("PUT_NAME_HERE","r");
    if(!file) {
        printf(COLOR_RED"(error) ERR failed to open file with database name\n"COLOR_RESET);
        return 1;
    }

    /* reading name */
    char name[BUFF_SIZE];
    fread(name,BUFF_SIZE,1,file);
    name[strcspn(name,"\n")] = '\0';
    fclose(file);

    HashMap *map = map_init();
    if(!map) return 1;

    load_aof(&map);

    char *buffer = NULL;
    size_t size = 0;

    // Welcome message
    printf(COLOR_CYAN COLOR_BOLD "\n╔═══════════════════════════════════════════════════════╗\n" COLOR_RESET);
    printf(COLOR_CYAN COLOR_BOLD "║" COLOR_RESET COLOR_BOLD COLOR_WHITE "            Welcome to Redis-like Database!            " COLOR_RESET COLOR_BOLD COLOR_CYAN "║\n" COLOR_RESET);
    printf(COLOR_CYAN COLOR_BOLD "╚═══════════════════════════════════════════════════════╝\n" COLOR_RESET);
    printf(COLOR_DIM "  Type 'help' for available commands or 'exit' to quit\n\n" COLOR_RESET);

    while(1) {
        command_t *command = parser_command_init();
        if(!command) {
            map_destroy(map);
            return 1;
        }

        printf(COLOR_YELLOW "[%s]> " COLOR_RESET,name);
        if(getline(&buffer,&size,stdin)==-1) {
            printf(COLOR_RED "(error) ERR input failure\n" COLOR_RESET);
            parser_command_destroy(command);
            continue;
        }

        buffer[strcspn(buffer,"\n")] = '\0';
        if(strlen(buffer)==0){ source_pos = 0; continue; }

        if(strcmp(buffer,"exit")==0) {
            printf(COLOR_CYAN "Exiting ...\n" COLOR_RESET);
            map_destroy(map);
            parser_command_destroy(command);
            free(buffer);
            size = 0;
            buffer = NULL;
            return 0;
        }

        if(parser_parse_command(command,buffer,source_pos)==-1) {
            parser_command_destroy(command);
            free(buffer);
            size = 0;
            buffer = NULL;
            source_pos = 0;
            continue;
        }

        if(execute(&map,command,EXECUTE_FROM_MAIN)==-1) {
            parser_command_destroy(command);
            free(buffer);
            size = 0;
            buffer = NULL;
            source_pos = 0;
            continue;
        }

        free(buffer);
        parser_command_destroy(command);
        source_pos = 0;
        size = 0;
        buffer = NULL;
    }

    return 0;
}
