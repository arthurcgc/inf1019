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


void firstExecution(Vector *line1, pid_t *fila1,  pid_t *fila2, pid_t *fila3)
{
    pid_t pid;
    int semId, segment, *child_pid, status;
    // aloca a memória compartilhada
    segment = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    // associa a memória compartilhada ao processo
    child_pid = (int*)shmat(segment, 0, 0); // comparar o retorno com -1
    semId = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
    setSemValue(semId);
        // initializing line head
    line1->curr = line1->begin;
    for(int i = 0; i < line1->size; i++)
    {
        printf("line1 size: %d\n", line1->size);
        printf("programa corrente = %s\n", line1->curr->program_name);
        pid = fork();
/*         if(pid == 0)
            break; */
        if(pid == 0)
        {
            *child_pid = getpid();
            //making sure the father doesn't access the wrong child pid
            printf("entered child process pid: %d\nExecuting program: %s...\n", getpid(), line1->curr->program_name);
            executeProgram(line1->curr);
        }
        
        if(pid > 0)
        {
            int stopped = 0;
            printf("entered father, pid = %d\n", getpid());
            // kill(*child_pid, SIGCONT);
            for(int start = 0; start < quantum; start++)
            {
                sleep(1);
                stopped = waitpid(*child_pid, &status, WUNTRACED|WNOHANG);
                if(stopped > 0)
                {
                    printf("[I/O BOUND] rajada[%d] chegou ao fim\n\n", line1->curr->itime);
                    line1->curr->itime++;
                    if(line1->curr->itime == line1->curr->time_sequence_tam)
                    {
                        pop_curr(line1);
                        break;
                    }
                    //processo é I/O -> manda pra fila 1
                    fila1[i] = *child_pid;
                    line1->curr = line1->curr->next;
                }
            }
            if(stopped > 0)
                continue;
            kill(*child_pid, SIGSTOP);
            printf("[CPU BOUND] stopping child pid = %d\n", *child_pid);
            // envia para fila2
            fila2[i] = *child_pid;
            if ( line1->curr->itime == line1->curr->time_sequence_tam)
                pop_curr(line1);
            line1->curr = line1->curr->next;
            // se só tem uma rajada->pop
        }
    }
    delSemValue(semId);
}

void printFila(pid_t * fila, int tam);

int main(int argc, char const *argv[])
{    

    Vector *line1;
    //creating vector
    line1 = create_vector(1);
    ReadCommands(line1);
    printPrograms(line1);

    pid_t *fila1, *fila2, *fila3;
    fila1 = (pid_t*)malloc(sizeof(pid_t)*line1->size);
    fila2 = (pid_t*)malloc(sizeof(pid_t)*line1->size);
    fila3 = (pid_t*)malloc(sizeof(pid_t)*line1->size);

    firstExecution(line1, fila1, fila2, fila3);

    printf("\nFila1:\n");
    printFila(fila1, line1->size);
    printf("\nFila2:\n");
    printFila(fila2, line1->size);
    printf("\nFila3:\n");
    printFila(fila3, line1->size);
    return 0;
}

void printFila(pid_t * fila, int tam)
{
    for(int i = 0; i < tam; i++)
    {
        printf("pid[%d] = %d\n", i, fila[i]);
    }
}