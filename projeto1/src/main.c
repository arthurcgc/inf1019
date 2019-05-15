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
#define DELTA 3

int ALARM_TRIGG = 0;
int CHILD_TRIGG = 0;


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

void childHandler(int signal)
{
    CHILD_TRIGG = 1;
}

void alarmHandler(int signal)
{
    ALARM_TRIGG = 1;
}

void executeProgram(Command *c)
{
    pid_t pid;
    char buffer[3];
    sprintf(buffer, "%d", c->time_sequence[c->inx_time]);
    c->inx_time++;
    char *parmList[] = {"program", buffer, NULL};

    
    execv(parmList[0], parmList);
}

void alarmSetUp(int pid, int delta)
{
    while(1)
    {
        if(ALARM_TRIGG)
        {
            printf("Alarm triggered!\n");
            int result = waitpid(pid, NULL, WNOHANG);
            if(result == 0)
            {
                // child still running so kill it
                printf("stopping child\n");
                kill(pid, SIGSTOP);
                ALARM_TRIGG = 0;
                CHILD_TRIGG = 0;
                alarm(delta);
                pause();
            }
            else
            {
                printf("alarm triggered but child exited normaly \n");
                exit(1);
            }
        }
        else if(CHILD_TRIGG)
        {
            kill(pid, SIGCONT);
        }
    }
}

int main(int argc, char const *argv[])
{    
    pid_t pid, child_pid;
    Vector *programs = create_vector();

    ReadCommands(programs);
    programs->curr = programs->begin;

    for(int i = 0; i < programs->size+1; i++)
    {
        pid = fork();
        if(pid == 0)
            break;
    }
    if(pid == 0)
    {
        child_pid = pid;
        executeProgram(programs->curr);
        // programs->curr = programs->curr->next;
    }
    signal(SIGALRM, alarmHandler);
    signal(SIGCHLD, childHandler);
    alarm(DELTA);
    pause();
    alarmSetUp(pid, DELTA);

    if(pid > 0)
    {
        
    }
    // executeProgram(programs->begin);

    return 0;
}
