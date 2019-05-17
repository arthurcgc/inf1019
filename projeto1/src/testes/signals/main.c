#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>


void executeProgram()
{
    char buffer[3];
    char *parmList[] = {"p1", NULL};
    execv(parmList[0], parmList);
}


int main(int argc, char const *argv[])
{
    pid_t pid;

    pid = fork();

    if(pid == 0)
    {
        executeProgram();
    }

    else if (pid > 0)
    {
        kill(pid, SIGSTOP);
        sleep(3);
    }

    return 0;
}

