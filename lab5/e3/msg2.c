#include <sys/sem.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <string.h>

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

int main(int argc, char const *argv[])
{
    int segmento, *p, id, pid, status;
    char *letter;
    int semId;

    // aloca a memória compartilhada
    segmento = shmget(8732, sizeof(char*), IPC_EXCL | S_IRUSR | S_IWUSR);
    // associa a memória compartilhada ao processo
    letter = (char*)shmat(segmento, 0, 0); // comparar o retorno com -1
    semId = semget(8752, 1, 0666);
    setSemValue(semId);

    while(*letter != '.')
    {
        semaforoP(semId);
        printf("%c\n", *letter + 1);
        semaforoV(semId);
        sleep(2);
    }


    delSemValue(semId);
    // libera a memória compartilhada do processo
    shmdt(p);
    // libera a memória compartilhada
    int i = shmctl(segmento, IPC_RMID, 0);

    return 0;
}