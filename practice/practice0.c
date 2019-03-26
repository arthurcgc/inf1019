#include <stdio.h>
#include <sys/types.h> 
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main()
{
    pid_t child_process = fork();

    if(child_process < 0)
    {
        perror("fork() error");
        exit(-1);
    }

    // if returns 0 it means we are at the child process
    if(child_process != 0)
    {
        printf("Im the parent %d, my child is %d\n", getpid(), child_process);
        wait(NULL); // waits for the child process to join this parent
    }

    else
    {
        printf("I'm the child %d, my parent is %d\n", getpid(), getppid());
        execl("/bin/echo","echo", "Hello World!", NULL);
    }

    return 0;
    
}