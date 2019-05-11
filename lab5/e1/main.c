#include <sys/sem.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/shm.h>
#include <sys/stat.h>


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

int main(int argc, char *argv[])
{
    int segmento, *p, id, pid, status;
    int semId;
    // aloca a memória compartilhada
    segmento = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    // associa a memória compartilhada ao processo
    p = (int *)shmat(segmento, 0, 0); // comparar o retorno com -1
    *p = 0;
    semId = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
    setSemValue(semId);
    if ((id = fork()) < 0)
    {
        puts("Erro na criação do novo processo");
        exit(-2);
    }
    else if (id == 0)
    {
        printf("[Processo filho] p inicial = %d\n", *p);
        semaforoP(semId);
        for(int i = 0; i < 10; i++)
        {
            *p += 1;
            sleep(1);
            printf("%d ", *p);fflush(stdout);
        }
        printf("\n");        
        semaforoV(semId);
    }
    else
    {
        printf("[Processo pai] p inicial = %d\n", *p);
        semaforoP(semId);
        for(int i = 0; i < 10; i++)
        {
            *p += 5;
            sleep(2);
            printf("%d ", *p);fflush(stdout);
        }
        printf("\n");        
        semaforoV(semId);
    }
    delSemValue(semId);    
    // libera a memória compartilhada do processo
    shmdt(p);
    // libera a memória compartilhada
    int i = shmctl(segmento, IPC_RMID, 0);
    return 0;
}