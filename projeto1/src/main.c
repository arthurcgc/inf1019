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
    char *parmList[] = {"program", buffer, NULL};    
    execv(parmList[0], parmList);
}


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
        // loop até completar uma rajada
        while(1)
        {
            kill(*child_pid, SIGCONT);
            if(pid > 0)
            {
                //child_status == 0 -> still running
                int child_status, start = 0;
                int flag_io = 0;
                while(start < quantum)
                {
                    sleep(1);
                    start++;
                    child_status = waitpid(*child_pid, &status, WNOHANG);
                    if(child_status == *child_pid)
                    {
                        // I/O process e fim da rajada
                        printf("Child is I/O bound\n");
                        flag_io = 1;
                        printf("rajada[%d] chegou ao fim\n\n", line1->curr->itime);
                        line1->curr->itime++;
                        //acabaram as rajadas
                        if(line1->curr->itime == line1->curr->time_sequence_tam)
                        {
                            pop_curr(line1);
                            break;
                        }
                        else
                        {
                            // pedido no enunciado
                            sleep(3);
                            send2back(line1);
                        }
                    }
                }
                if(flag_io)
                {
                    flag_io = 0;
                    break;
                }
                printf("entered father, pid = %d\n", getpid());
                printf("[CPU BOUND] stopping child pid = %d\n", *child_pid);
                kill(*child_pid, SIGSTOP);
                
                // envia para o final da fila
                if ( line1->curr->itime < line1->curr->time_sequence_tam)
                {
                    line1->curr->time_sequence[line1->curr->itime] -= quantum;
                    if(line1->curr->time_sequence[line1->curr->itime] <= 0)
                    {
                        printf("rajada[%d] chegou ao fim\n\n", line1->curr->itime);
                        line1->curr->itime++;
                        if(line1->curr->itime == line1->curr->time_sequence_tam)
                        {
                            pop_curr(line1);
                            break;
                        }
                        line1->curr->time_sequence[line1->curr->itime] = 0;
                        break;
                    }
                    else
                        printf("tempo restante da rajada = %ds\n\n", line1->curr->time_sequence[line1->curr->itime]);
                    send2back(line1);
                }
                else
                {
                    // rajadas esgotadas, hora de remover do vetor
                    pop_curr(line1);
                }
            }
        }
    }

    delSemValue(semId);
    return 0;
}
