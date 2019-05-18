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
#include "semaforo.h"

#define line_cap 256
#define command_cap 50
#define quantum 3

int IO_FLAG = 0;

char *my_itoa(int num, char *str)
{
        if(str == NULL)
        {
                return NULL;
        }
        sprintf(str, "%d", num);
        return str;
}


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
    int i = 1;
    pid_t pid;
    char *buffer[c->time_sequence_tam+2];
    buffer[0] = "program";
    for(; i < c->time_sequence_tam+1; i++)
    {
        char temp[2];
        // sprintf(temp, "%d", time_sequence[i-1]);
        my_itoa(c->time_sequence[i-1], temp);
        buffer[i] = (char*)malloc(sizeof(char)*2);
        strcpy(buffer[i], temp);
        // printf("buffer[%d] = %s\n", i ,buffer[i]);
    }
    // printf("adding NULL\n\n");
    buffer[i] = (char*)malloc(sizeof(char)*2);
    buffer[i] = NULL;
    // char *parmList[] = {"program", buffer, NULL};    
    execv("program", buffer);
}

void w4IO(int signal);

int main(int argc, char const *argv[])
{    
    pid_t pid;
    int semId, segment, *child_pid, status;
    Vector *line1;

    // aloca a memória compartilhada
    segment = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    // associa a memória compartilhada ao processo
    child_pid = (int*)shmat(segment, 0, 0); // comparar o retorno com -1
    semId = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
    setSemValue(semId);

    //creating vector
    line1 = create_vector();
    ReadCommands(line1);
    printPrograms(line1);

    // initializing line head
    while(line1->size > 0)
    {
        printf("line1 size: %d\n", line1->size);
        line1->curr = line1->begin;
        printf("programa corrente = %s\n", line1->curr->program_name);
        signal(SIGCHLD, w4IO);
        pid = fork();
/*         if(pid == 0)
            break; */
        if(pid == 0)
        {
            IO_FLAG = 0;
            *child_pid = getpid();
            //making sure the father doesn't access the wrong child pid
            printf("entered child process pid: %d\nExecuting program: %s...\n", getpid(), line1->curr->program_name);
            executeProgram(line1->curr);
        }
        
        while(1)
        {
            IO_FLAG = 0;
            kill(*child_pid, SIGCONT);
            if(pid > 0)
            {
                printf("entered father, pid = %d\n", getpid());
                //kill(*child_pid, SIGCONT);
                for(int start = 0; start < quantum; start++)
                {
                    sleep(1);
                    if(IO_FLAG)
                    {
                        printf("[I/O BOUND] rajada[%d] chegou ao fim\n\n", line1->curr->itime);
                        line1->curr->itime++;
                        if(line1->curr->itime == line1->curr->time_sequence_tam)
                        {
                            pop_curr(line1);
                            break;
                        }
                        send2back(line1);
                        break;
                    }
                }
                if(IO_FLAG)
                {
                    continue;
                }
                printf("[CPU BOUND] stopping child pid = %d\n", *child_pid);
                kill(*child_pid, SIGSTOP);
                // envia para o final da fila
                if ( line1->curr->itime < line1->curr->time_sequence_tam)
                {
                    line1->curr->time_sequence[line1->curr->itime] -= quantum;
                    if(line1->curr->time_sequence[line1->curr->itime] <= 0)
                    {
                        printf("rajada[%d] chegou ao fim\n\n", line1->curr->itime);
                        line1->curr->time_sequence[line1->curr->itime] = 0;
                        line1->curr->itime++;
                        if(line1->curr->itime == line1->curr->time_sequence_tam)
                        {
                            pop_curr(line1);
                            break;
                        }
                    }
                    else
                        printf("tempo restante da rajada[%d] = %ds\n\n",line1->curr->itime
                        ,line1->curr->time_sequence[line1->curr->itime]);

                    send2back(line1);
                }
            }
        }
    }

    delSemValue(semId);
    return 0;
}



void w4IO(int signal)
{
    printf("entered Handler\n");
    IO_FLAG = 1;
}