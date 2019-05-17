#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>

int main(int argc, char const *argv[])
{
    int i = 0;
    while(1)
    {
        printf("%d\n", i);
        sleep(2);
        i++;
    }
    return 0;
}
