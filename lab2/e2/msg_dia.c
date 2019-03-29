#include <stdio.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>


int main(int argc, char *argv[])
{
    int id, pid, status;
    char *msg = (char*)malloc(256*sizeof(char));
    int segmento;
    int *p = (int*)malloc(sizeof(msg));

    printf("Digite a msg do dia:\n");
    scanf(" %[^\n]", msg);

    
    // aloca a memória compartilhada
    segmento = shmget(8765, sizeof(char)*256, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    // associa a memória compartilhada ao processo
    p = (int *)shmat(segmento, 0, 0); // comparar o retorno com -1
    strcpy((char*)p, msg);

    return 0;
}