#include <stdio.h>
#include <sys/types.h> 
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char const *argv[])
{
    if(argc < 2)
    {
        return (EXIT_FAILURE);
    }
    for(int i = 1; i < argc;  i++)
        printf("%s ", argv[i]);
    printf("\n");
    return 0;
}