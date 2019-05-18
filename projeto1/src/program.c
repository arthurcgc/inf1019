#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    int time;
    for(int i = 1;i < argc; i++)
    {
        // printf("argv[i] = %s\n", argv[i]);
        time = argv[i][0] - '0';
        // printf("time = %d\n", time);
        for(int j = 0; j < time; j++)
        {
            printf("%d\n", getpid());
            sleep(1);
        }
        printf("enviando sinais\n");
        // kill(getppid(), SIGCHLD);
        kill(getpid(), SIGSTOP);
    }

    return 0;
}
