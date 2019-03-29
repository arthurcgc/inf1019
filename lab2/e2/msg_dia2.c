#include <stdio.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>


int main(int argc, char const *argv[])
{
    int segmento, *p;

    segmento = shmget(8765, sizeof(char)*256, IPC_EXCL | S_IRUSR | S_IWUSR);
    p = (int*)shmat(segmento, 0, 0);

    printf("%s\n", (char*) p);



    // libera a memória compartilhada do processo
    shmdt(p);
    // libera a memória compartilhada
    shmctl(segmento, IPC_RMID, 0);
    return 0;
}
