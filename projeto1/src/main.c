#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "vector.h"

#define line_cap 256
#define command_cap 50


void printPrograms(Vector *v)
{
    Command *curr = v->begin;
    for(int i = 0; i < size(v); i++)
    {
        printf("%s (%d, %d, %d)\n", curr->program_name, curr->time_sequence[0],
        curr->time_sequence[1], curr->time_sequence[2]);
        curr = curr->next;
    }
}

void ReadCommands(Vector *programs)
{
    char *line = (char*)malloc(line_cap*sizeof(char));
    char *p_name = (char*)malloc(line_cap*sizeof(char));
    char *time_windows = (char*)malloc(line_cap*sizeof(char));
    int end = 0;

    for(int i = 0; i < command_cap; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            scanf(" %s", line);
            if(strcmp(line, "exec") == 0)
                continue;
            if(strcmp(line, "end") == 0)
            {
                end = 1;
                break;
            }
            if(line[0] != '(')
            {
                strcpy(p_name, line);
            }
            else if(line[0] == '(')
            {
                strcat(time_windows, line);
                fgets(line, sizeof(line), stdin);
                strcat(time_windows, line);
            }
        }
        if(end)
            break;
        Command *c = create_command(p_name, time_windows);
        push_back(programs, c);
        memset(time_windows,0,sizeof(time_windows));
    }
}

int main(int argc, char const *argv[])
{    
    Vector *programs = create_vector();

    ReadCommands(programs);
    printPrograms(programs);


    return 0;
}
