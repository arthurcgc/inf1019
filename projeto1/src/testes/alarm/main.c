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

void treatAlarm(int signal)
{
    ALARM_TRIGG = 1;
}

void childHandler(int signal)
{
    CHILD_TRIGG = 1;
}

int main(int argc, char const *argv[])
{
    pid_t pid, child_pid;
    int status;
    pid = fork();

    if(pid == 0)
    {
        executeProgram();
        child_pid = pid;
    }
    
    signal(SIGALRM, treatAlarm);
    signal(SIGCHLD, childHandler);
    alarm(8);
    pause();

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
                alarm(8);
                pause();
            }
            else
            {
                printf("alarm triggered but child exited normally\n");
            }
        }
        else
        {
            kill(pid, SIGCONT);
        }
    }
    return 0;
}
