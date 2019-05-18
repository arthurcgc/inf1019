#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

int SIGUSR1_FLAG = 0;
char *my_itoa(int num, char *str)
{
        if(str == NULL)
        {
                return NULL;
        }
        sprintf(str, "%d", num);
        return str;
}

void executeProgram(int time_sequence[10], int tam)
{
    int i = 1;
    pid_t pid;
    char *buffer[tam+2];
    buffer[0] = "program";
    for(; i < tam+1; i++)
    {
        char temp[2];
        // sprintf(temp, "%d", time_sequence[i-1]);
        my_itoa(time_sequence[i-1], temp);
        buffer[i] = (char*)malloc(sizeof(char)*2);
        strcpy(buffer[i], temp);
        printf("buffer[%d] = %s\n", i ,buffer[i]);
    }
    // printf("adding NULL\n\n");
    buffer[i] = (char*)malloc(sizeof(char)*2);
    buffer[i] = NULL;
    // char *parmList[] = {"program", buffer, NULL};    
    execv("program", buffer);
}

void treatSIGUSR1(int signal)
{
    printf("I/O bound\n");
}


int main(int argc, char const *argv[])
{
    pid_t pid;

    int ts[10], tam = 4;
    ts[0] = 5;
    ts[1] = 2;
    ts[2] = 1;
    ts[3] = 4;

    //pid = fork();

/*     if(pid == 0)
    {
        executeProgram(ts, tam);
    }

    else if (pid > 0)
    {
        signal(SIGUSR1, treatSIGUSR1);
        printf("pid pai = %d\n", getpid());
        sleep(10);
    } */

    executeProgram(ts, tam);
    return 0;
}

