#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    int time;
    for(int i = 1;i < argc-1; i++)
    {
        sscanf(argv[i], "%d", &time);
        for(int j = 0; j < time; j++)
        {
            printf("%s - pid: %d - rajada[%d] - tempo restante da rajada: %d\n",argv[argc-1] ,getpid(), i, time-j);
            sleep(1);
        }
        printf("enviando sinais\n");
        kill(getpid(), SIGSTOP);
    }
    exit(0);

    return 0;
}
