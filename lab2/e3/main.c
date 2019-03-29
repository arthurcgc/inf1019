#include <stdio.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>


int main(int argc, char *argv[])    
{
    int segmento, pid1, pid2;
    int *p;
    int count = 0;
    int resp = 0;

    // aloca a memória compartilhada
    segmento = shmget(IPC_PRIVATE, sizeof(int)*1000, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    // associa a memória compartilhada ao processo
    p = (int *)shmat(segmento, 0, 0); // comparar o retorno com -1



    srand(time(0));
    for(int i =0; i < 1000; i++)
    {
        p[i] = rand() % 100;
        if(p[i] == 99)
            count ++;
    }
    printf("resposta esperada: %d\n", count);

    pid1 = fork();
    pid2 = fork();

    if(pid1 > 0 && pid2 > 0)
    {
        //parent
        printf("De [0,249]: ");
        int *temp = &p[0];
        for(int i = 0; i < 250; i++)
        {
            temp++;
            if(*temp == 99)
            {
                resp++;
                printf("%p - ", temp);
            }
        }

    }

    else if(pid1 == 0 && pid2 > 0)
    {
        //first child
        printf("De [250,499]: ");
        int *temp = &p[0]+250; // + elem*500;
        for(int i = 0; i < 250; i++)
        {
            temp ++;
            if(*temp == 99)
            {
                resp++;
                printf("%p - ", temp);
            }
        }
    }

    else if(pid1 > 0 && pid2 == 0)
    {
        //second
        printf("De [500, 749]: ");
        int *temp = &p[0]+500; // + elem*500;
        for(int i = 0; i < 250; i++)
        {
            temp ++;
            if(*temp == 99)
            {
                resp++;
                printf("%p - ", temp);
            }
        }
    }

    else if(pid1 == 0 && pid2 == 0)
    {
        //third child
        printf("De [750, 1000]: ");
        int *temp = &p[0]+750;
        for(int i = 0; i <= 250; i++)
        {
            temp ++;
            if(*temp == 99)
            {
                resp++;
                printf("%p - ", temp);
            }
        }
    }
    

    printf("Total: %d\n", resp);
    
    // libera a memória compartilhada do processo
    shmdt(p);
    // libera a memória compartilhada
    shmctl(segmento, IPC_RMID, 0);
    return 0;
}