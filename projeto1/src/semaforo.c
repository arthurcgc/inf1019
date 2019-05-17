#include "semaforo.h"


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