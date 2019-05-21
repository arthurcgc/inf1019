#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h> 
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/wait.h>
#include "vector.h"
#include "parser.h"

#define quantum 3


void executeProgram(Command *c)
{
    int i = 1;
    pid_t pid;
    char *buffer[c->time_sequence_tam+3];
    buffer[0] = "program";
    for(; i < c->time_sequence_tam+1; i++)
    {
        char temp[2];
        my_itoa(c->time_sequence[i-1], temp);
        buffer[i] = (char*)malloc(sizeof(char)*2);
        strcpy(buffer[i], temp);
    }
    buffer[i] = (char*)malloc(sizeof(char)*256);
    strcpy(buffer[i], c->program_name);
    buffer[i+1] = (char*)malloc(sizeof(char)*2);
    buffer[i+1] = NULL;
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
    
    // initializing line head
    line1->curr = line1->begin;
    for(int i = 0; i < line1->size; i++)
    {
        pid = fork();

        if(pid == 0)
        {
            *child_pid = getpid();
            //making sure the father doesn't access the wrong child pid
            executeProgram(line1->curr);
        }
        
        if(pid > 0)
        {
            int stopped = 0;
            int flag_cont = 0;
            for(int start = 0; start < quantum; start++)
            {
                sleep(1);
                stopped = waitpid(*child_pid, &status, WUNTRACED|WNOHANG);
                if(stopped > 0)
                {
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
        }
    }
     // libera a memória compartilhada do processo
    shmdt(child_pid);
    // libera a memória compartilhada
    int i = shmctl(segment, IPC_RMID, 0);
}


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
                {
                    printf("processo %d continua na fila3\n\n", fila_corr[j]);
                    continue;
                }
            }
        }
    }
    return count_terminated;
}

void printFila(pid_t * fila, int tam);

int main(int argc, char const *argv[])
{    

    Vector *line1;
    //creating vector
    line1 = create_vector(1);
    ReadFile(line1);
    printPrograms(line1);

    // creating lines
    pid_t *fila1, *fila2, *fila3;
    fila1 = (pid_t*)malloc(sizeof(pid_t)*line1->size);
    fila2 = (pid_t*)malloc(sizeof(pid_t)*line1->size);
    fila3 = (pid_t*)malloc(sizeof(pid_t)*line1->size);

    printf("-------------Fila1-------------\n");
    firstExecution(line1, fila1, fila2, fila3);


    // percorre a fila1 tres vezes
    int count_terminated = 0;
    while(count_terminated < line1->size)
    {
        // é 3 por causa da primeira execução
        printf("-------------Fila1-------------\n");
        for(int i = 0; i < 4; i++)
            count_terminated += rodaFila(fila1, fila2, fila3, 1, line1->size);
        printf("------------Fila2--------------\n");
        for(int i = 0; i < 2; i++)
            count_terminated += rodaFila(fila1, fila2, fila3, 2, line1->size);
        printf("------------Fila3--------------\n");
        count_terminated += rodaFila(fila1, fila2, fila3, 3, line1->size);
    }

    return 0;
}

