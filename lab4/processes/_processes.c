#include <stdio.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <math.h>
#include <time.h>
#include <signal.h>

#define v1_size 1000
#define v2_size 4000
#define v3_size 16000
#define pnum    10

//creates random vector and returns its highest value
int create_rand_vector(int *v, int m)
{
    int highest = 0;
    for(int i =0; i < m; i++)
    {
        v[i] = rand();
        if(v[i] > highest)
            highest = v[i];
    }
    return highest;
}

int get_highest(int *v, int limit)
{
    int high_temp = 0;
    for(int i = 0; i < limit; i++)
    {
        if(v[i] > high_temp)
            high_temp = v[i];
    }
    return high_temp;
}

    clock_t start, end;

int solveWithProcesses(int *v, int size, int threshold, int pid)
{
    int cand = 0;
    int status;
    int highest = 0;
    start = clock();
    if(pid == 0)
    {
        cand = get_highest(v, size);
        if(cand > highest)
            highest = cand;
    }
    else
    {
        //parent of pids[i]
        cand = get_highest(v + threshold, size);
        if(cand > highest)
            highest = cand;
    }

    end = clock();
    return highest;
}


int main(int argc, char *argv[])    
{
    int segmento, resp, rp;
    int *p1, *p2, *p3;
    int pids[4];
    int threshold1 = abs((v1_size+24)/16);
    int threshold2 = abs((v2_size)/16);
    int threshold3 = abs((v3_size)/16);
    double cpu_time_ms;

    // aloca a memória compartilhada
    segmento = shmget(IPC_PRIVATE, sizeof(int)*(v1_size+24), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);

    // associa a memória compartilhada ao processo
    p1 = (int *)shmat(segmento, 0, 0); // comparar o retorno com -1

    srand(time(0));

    resp = create_rand_vector(p1, v1_size);

    printf("\nanswer expected: %d\n", resp);
    // 4 fork() = 2^4 processes, totalizing 16 processes    
    rp = 0;
    for (int i = 0; i < 4; i++)
    {
        pids[i] = fork();
        int cand = solveWithProcesses(p1+(2*i*threshold1), v1_size, threshold1, pids[i]);
        if(cand > rp)
        {
            rp = cand;
            if(rp == resp)
            {
                printf("\nanswer of #pid%d: %d\n", pids[i], rp);
                kill(pids[i], SIGKILL);
                end = clock();
                cpu_time_ms = ((double) (end - start) / (CLOCKS_PER_SEC)) * 1000;
                printf("Tempo de execução: %f\n", cpu_time_ms);
            }
        }
    }

    

    // libera a memória compartilhada do processo
    shmdt(p1);
    // libera a memória compartilhada
    shmctl(segmento, IPC_RMID, 0);


    return 0;
}