#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    int time = atoi(argv[1]);
    for(int i = 0; i < time; i++)
    {
        printf("%d\n", getpid());
        sleep(1);
    }

    exit(0);
}
