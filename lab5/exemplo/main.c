#include <sys/sem.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

union semun
{
    int val; // valor do semáforo
    struct semid_ds *buf; // operação do semáforo
    unsigned short *array; // flags da operação
};

// inicializa o valor do semáforo
int setSemValue(int semId)
{
    union semun semUnion;
    semUnion.val = 1;
    return semctl(semId, 0, SETVAL, semUnion);
}

// remove o semáforo
void delSemValue(int semId)
{
    union semun semUnion;
    semctl(semId, 0, IPC_RMID, semUnion);
}

// operação P
int semaforoP(int semId)
{
    struct sembuf semB;
    semB.sem_num = 0;
    semB.sem_op = -1;
    semB.sem_flg = SEM_UNDO;
    semop(semId, &semB, 1);
    return 0;
}

//operação V
int semaforoV(int semId)
{
    struct sembuf semB;
    semB.sem_num = 0;
    semB.sem_op = 1;
    semB.sem_flg = SEM_UNDO;
    semop(semId, &semB, 1);
    return 0;
}

int main (int argc, char * argv[])
{
    int i;
    char letra = 'o';
    int semId;
    if (argc > 1)
    {
        semId = semget (8752, 1, 0666 | IPC_CREAT);
        setSemValue(semId);
        letra = 'x';
        sleep (2);
    }
    else
    {
        while ((semId = semget (8752, 1, 0666)) < 0)
        {
            putchar ('.'); fflush(stdout);
            sleep (1);
        }
    }
    for (i=0; i<10; i++)
    {
        semaforoP(semId);
        putchar (toupper(letra)); fflush(stdout);
        sleep (rand() % 3);
        putchar (letra); fflush(stdout);
        semaforoV(semId);
        sleep (rand() % 2);
    }
    printf ("\nProcesso %d terminou\n", getpid());
    if (argc > 1)
    {
        sleep(10);
        delSemValue(semId);
    }

    delSemValue(semId);    
    return 0;
}

