#include <stdio.h>
#include <string.h>
#include "../header/commands.h"

static int source_pos = 0;

int main(void) {
    HashMap *map = map_init();
    if(!map) return 1;

    char *buffer = NULL;
    size_t size = 0;

    while(1) {
        command_t *command = parser_command_init();
        if(!command) {
            map_destroy(map);
            return 1;
        }

        printf(">> ");
        if(getline(&buffer,&size,stdin)==-1) {
            printf("Input failure\n");
            parser_command_destroy(command);
            continue;
        }

        buffer[strcspn(buffer,"\n")] = '\0';
        if(strlen(buffer)==0){ source_pos = 0; continue; }

        if(strcmp(buffer,"exit")==0) {
            printf("Exiting ...\n");
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

        if(execute(&map,command)==-1) {
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
