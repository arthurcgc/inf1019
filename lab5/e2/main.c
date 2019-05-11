#include <sys/sem.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <string.h>

#define MAX_CHAR 16
#define MAX_ITER 64

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
    char *buffer = (char*)malloc(sizeof(char)*MAX_CHAR); 
    int semId;
    // aloca a memória compartilhada
    segmento = shmget(IPC_PRIVATE, sizeof(char)*MAX_CHAR, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    // associa a memória compartilhada ao processo
    buffer = (char *)shmat(segmento, 0, 0); // comparar o retorno com -1
    semId = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
    setSemValue(semId);

    if ((id = fork()) < 0)
    {
        puts("Erro na criação do novo processo");
        exit(-2);
    }
    else if (id == 0)
    {
        // consumidor, aka filho
        for(int i = 0; i < MAX_ITER; i++)
        {
            semaforoP(semId);
            printf("%s\n", buffer);
            memset(buffer, 0, sizeof(buffer));
            semaforoV(semId);
        }
    }
    else
    {
        // produtor, aka pai
        for(int i = 0; i < MAX_ITER; i++)
        {
            semaforoP(semId);
            for(int j = 0; j <= MAX_CHAR; j++)
                buffer[j] = getchar();
            
            semaforoV(semId);
        }
    }

    delSemValue(semId);
    // libera a memória compartilhada do processo
    shmdt(p);
    // libera a memória compartilhada
    int i = shmctl(segmento, IPC_RMID, 0);
    return 0;
}