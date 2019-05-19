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
#include "parser.h"

#define quantum 4

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
        //printf("line1 size: %d\n", line1->size);
        pid = fork();
/*         if(pid == 0)
            break; */
        if(pid == 0)
        {
            *child_pid = getpid();
            //making sure the father doesn't access the wrong child pid
            // printf("entered child process pid: %d\nExecuting program: %s...\n", getpid(), line1->curr->program_name);
            executeProgram(line1->curr);
        }
        
        if(pid > 0)
        {
            int stopped = 0;
            // printf("entered father, pid = %d\n", getpid());
            int flag_cont = 0;
            for(int start = 0; start < quantum; start++)
            {
                sleep(1);
                stopped = waitpid(*child_pid, &status, WUNTRACED|WNOHANG);
                if(stopped > 0)
                {
                    //printf("[I/O BOUND] rajada[%d] chegou ao fim\n\n", line1->curr->itime);
                    printf("processo %d é I/O BOUND\n\n", *child_pid);
                    line1->curr->itime++;
                    if(line1->curr->itime == line1->curr->time_sequence_tam)
                    {
                        pop_curr(line1);
                        break;
                    }
                    //processo é I/O -> manda pra fila 1
                    fila1[i] = *child_pid;
                    line1->curr = line1->curr->next;
                    flag_cont = 1;
                    break;
                }
            }
            if(flag_cont)
                continue;
            
            kill(*child_pid, SIGSTOP);
            printf("processo %d é CPU BOUND\n\n", *child_pid);
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


int isIO(pid_t pid)
{
    int stopped = 0;
    int status;
    for(int start = 0; start < quantum; start++)
    {
        sleep(1);
        stopped = waitpid(pid, &status, WUNTRACED|WNOHANG);
        if(stopped > 0)
        {
            printf("processo %d é I/O BOUND\n", pid);
            return 1;
        }
    }
    return 0;
}

int rodaFila(pid_t *fila1, pid_t *fila2, pid_t *fila3 , int fila_index, int n_processes)
{
    pid_t *fila_corr = (pid_t*)malloc(sizeof(pid_t)*n_processes);
    int count_terminated = 0;

    if(fila_index == 1)
        fila_corr = fila1;
    else if(fila_index == 2)
        fila_corr = fila2;
    else
        fila_corr = fila3;

    for(int j = 0; j < n_processes; j++)
    {
        int res = -2;
        if(fila_corr[j] == 0)
            continue;
        else
        {
            int terminated;
            int status;
            terminated = kill(fila_corr[j], SIGCONT);

            if(terminated == -1)
            {
                count_terminated++;
                printf("processo %d terminou\n\n", fila_corr[j]);
                fila_corr[j] = 0;
                continue;
            }
            //processo continua I/O bound -> mantém
            res = isIO(fila_corr[j]);
            if(res)
            {
                if(fila_index == 1)
                {
                    printf("processo %d continua na fila1\n\n", fila_corr[j]);
                    continue;
                }
                if(fila_index == 2)
                {
                    fila1[j] = fila_corr[j];
                    printf("processo %d subiu para fila1\n\n", fila_corr[j]);
                    fila_corr[j] = 0;
                }
                else
                {
                    fila2[j] = fila_corr[j];
                    printf("processo %d subiu para fila2\n\n", fila_corr[j]);
                    fila_corr[j] = 0;
                }
                
            }
            else
            {
                // é CPU BOUND
                kill(fila_corr[j], SIGSTOP);
                printf("processo %d é CPU BOUND\n", fila_corr[j]);
                // indice j -> ordem de execução do processo quando executamos a line1
                if(fila_index == 1)
                {
                    printf("Descendo processo %d para fila2\n\n", fila_corr[j]);
                    fila2[j] = fila_corr[j];
                    fila_corr[j] = 0;
                }
                else if(fila_index == 2)
                {
                    printf("Descendo processo %d para fila3\n\n", fila_corr[j]);
                    fila3[j] = fila_corr[j];
                    fila_corr[j] = 0;
                }
                else
                    continue;
            }
        }
    }
    return count_terminated;
}


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
    printf("___________END OF FIRST EXECUTION____________\n\n");


    // percorre a fila1 tres vezes
    int count_terminated = 0;
    while(count_terminated < line1->size)
    {
        // é 3 por causa da primeira execução
        printf("Fila1:\n");
        for(int i = 0; i < 4; i++)
            count_terminated += rodaFila(fila1, fila2, fila3, 1, line1->size);
        printf("Fila2:\n");
        for(int i = 0; i < 2; i++)
            count_terminated += rodaFila(fila1, fila2, fila3, 2, line1->size);
        printf("Fila3:\n");
        count_terminated += rodaFila(fila1, fila2, fila3, 3, line1->size);
    }

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

/*
    old_roda_fila1()
    for(int i = 0; i < 3; i++)
        {
            for(int j = 0; j < line1->size; j++)
            {
                int res = -2;
                if(fila1[j] == 0)
                    continue;
                else
                {
                    int terminated;
                    int status;
                    terminated = kill(fila1[j], SIGCONT);

                    if(terminated == -1)
                    {
                        count_terminated++;
                        printf("processo %d terminou\n", fila1[j]);
                        fila1[j] = 0;
                        continue;
                    }
                    //processo continua I/O bound -> mantém
                    res = isIO(fila1[j]);
                    if(res)
                        continue;
                    else
                    {
                        // é CPU BOUND
                        kill(fila1[j], SIGSTOP);
                        printf("processo %d é CPU BOUND\n\n", fila1[j]);
                        // indice j -> ordem de execução do processo quando executamos a line1
                        fila2[j] = fila1[j];
                        fila1[j] = 0;
                    }
                }
            }
        }
    } */