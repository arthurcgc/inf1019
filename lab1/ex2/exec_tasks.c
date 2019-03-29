#include <stdio.h>
#include <sys/types.h> 
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char const *argv[])
{
    char *parmList1[] = {"echoc" ,"arthur coelho", 0};
    char *parmList2[] = {"ola", 0};

    pid_t child = fork();
    
    if(child == 0)
        execv(parmList1[0], parmList1);
    else
        execv(parmList2[0], parmList2);

    return 0;
}
