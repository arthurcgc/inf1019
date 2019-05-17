#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h> 
#include <errno.h>
#include <sys/wait.h>
#include "vector.h"

#define line_cap 256
#define command_cap 50
#define quantum 3

int ALARM_TRIGG = 0;
int CHILD_TRIGG = 0;


void printPrograms(Vector *v)
{
    Command *curr = v->begin;
    for(int i = 0; i < v->size; i++)
    {
        printf("%s (", curr->program_name);fflush(stdout);
        for(int j = 0; j < curr->time_sequence_tam-1; j++)
        {
            printf("%d, ", curr->time_sequence[j]);
        }
        printf("%d)\n", curr->time_sequence[curr->time_sequence_tam-1]);
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
                return;
            }
            if(line[0] != '(')
            {
                strcpy(p_name, line);
            }
            else if(line[0] == '(')
            {
                strcat(time_windows, line);
                fgets(line, line_cap, stdin);
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

void executeProgram(Command *c)
{
    pid_t pid;
    char buffer[3];
    sprintf(buffer, "%d", c->time_sequence[c->itime]);
    c->itime++;
    char *parmList[] = {"program", buffer, NULL};    
    execv(parmList[0], parmList);
}


int main(int argc, char const *argv[])
{    
    pid_t pid, child_pid;
    Vector *line1 = create_vector();

    ReadCommands(line1);
    line1->curr = line1->begin->next;
    pop_curr(line1);
    printPrograms(line1);


    // line1->curr = line1->begin;
    /*
    for(int i = 0; i < line1->size+1; i++)
    {
        pid = fork();
        if(pid == 0)
            break;
    }
    if(pid == 0)
    {
        child_pid = pid;
        executeProgram(line1->curr);
        line1->curr = line1->curr->next;
        if(line1->curr == NULL)
        {
            // chegou ao fim da lista
            line1->curr = line1->begin;
            // other commands

        }
    }

    if(pid > 0)
    {
        
    }
    // executeProgram(line1->begin);
    */
    return 0;
}
