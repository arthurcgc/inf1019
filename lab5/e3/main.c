#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>


int main(int argc, char const *argv[])
{
    pid_t pid;
    char *const parmlist1[] = {"./msg1", NULL};
    char *const parmlist2[] = {"./msg2", NULL};

    pid = fork();

    if(pid == 0)
    {
        execv("./msg2", parmlist1);
    }
    else if(pid > 0)
    {
        execv("./msg1", parmlist2);
    }


    return 0;
}
