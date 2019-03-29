#include <stdio.h>
#include <sys/types.h> 
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>


int main()
{
    pid_t child= fork();
    pid_t grand_child;
    pid_t endID;
    int status;

    // if > 0 then, parent
    if(child > 0)   
    {
        for(int i = 0; i < 100; i++)
        {
            printf("count: %d - PID: %d\n", i, getpid());
            sleep(1);
        }
        endID = waitpid(child, &status, 0);
        printf("processo pai vai finalizar\n");
        if(endID == -1)
        {
            perror("\nwaitpid error\n");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }

    // if child == 0, then child
    else if(child == 0)
    {
        printf("Filho foi criado\n");

        /************************************************ INICIO DO NETO ***************************/
        grand_child = fork();
        if(grand_child == 0)
        {
            printf("Neto foi criado\n");
            for(int i = 200; i < 300; i++)
            {
                printf("count: %d - PID: %d - PID pai: %d\n", i, getpid(), getppid());
                sleep(3);
            }
            printf("Processo neto vai finalizar\n");
            exit(EXIT_SUCCESS);
        }
        /* *************************** FIM DO NETO *************************************************/


        for(int i = 100; i < 200; i++)
        {
            printf("count: %d - PID: %d - PID pai: %d\n", i, getpid(), getppid());
            sleep(2);
        }

        endID = waitpid(grand_child, &status, 0);
        printf("Processo filho vai finalizar\n");
        if(endID == -1)
        {
            perror("\nwaitpid error\n");
            exit(EXIT_FAILURE);
        }
        // grand_child = fork();
        exit(EXIT_SUCCESS);
    }

    return 0;
}
