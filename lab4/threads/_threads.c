#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define MILLION 1E-6
#define v1_size 1000
#define v2_size 4000
#define v3_size 16000

typedef struct args
{
    pthread_t tid;
    int *v;
} Args;

typedef struct time
{
    time_t tv_sec;
    time_t tv_nsec;
} Time;

int highest = 0;
int lowest = 2147483647;

void deepCopy(int *v1, int *v2, int size)
{
    for(int i = 0; i < size; i++)
        v2[i] = v1[i];
}


void *getHigh_Low(void *p)
{
    Args *temp = (Args*) p;
    int cand;
    int *v_temp;
    v_temp = temp->v;
    for(int i = 0; i < 100; i++)
    {
        cand = *v_temp;
        if(cand > highest)
            highest = cand;
        if(cand < lowest)
            lowest = cand;
        v_temp++;
    }
    pthread_exit(NULL);
}



void generate_random_vector(int *v, int size)
{
    for(int i = 0; i < size; i++)
        v[i] = rand();
}

int linearHighest(int *v, int size)
{
    int resp = *v;
    for(int i = 0; i < size; i++)
    {
        if(v[i]>resp)
            resp = v[i];
    }
    return resp;
}

int linearLowest(int *v, int size)
{
    int resp = *v;
    for(int i = 0; i < size; i++)
    {
        if(v[i]< resp)
            resp = v[i];
    }

    return resp;
}

void solveWithThreads(int *v, int size)
{
    int threads_num = abs(ceil(size/100));
    pthread_t threads[threads_num];
    highest = v[0];

    printf("\nthreads to be created: %d\n", threads_num);
    for(int i = 0; i < threads_num; i++)
    {
        threads[i] = i+1;
        Args *temp = (Args*)malloc(sizeof(Args));
        temp->tid = threads[i];
        temp->v = v;
        pthread_create(&threads[i], NULL, (void*) getHigh_Low , (void*) temp);
        v+=100;
    }
    
    for (int i = 0; i < threads_num; i++)
    {
        pthread_join(threads[i], NULL);
    }
    
}

void ThreadResolve(int *v, int size, int high, int low)
{
    Time start, end;
    double cpu_time_ms;
    clock_gettime(CLOCK_MONOTONIC, &start);
    solveWithThreads(v, size);
    clock_gettime(CLOCK_MONOTONIC, &end);
    cpu_time_ms = (double) (end.tv_nsec - start.tv_nsec)*MILLION/ (CLOCKS_PER_SEC);
    printf("\nmaior sem threads: %d\n", high);
    printf("menor sem threads: %d\n", low);
    printf("\nmaior com threads: %d\n", highest);
    printf("menor com threads: %d\n", lowest);
    if(high == highest)
        printf("numeros iguais\n\nTempo de execução: %f ms\n", cpu_time_ms);
    highest = 0;
    lowest = 2147483647;
    printf("----------------------------------------\n");
}

int main(int argc, char const *argv[])
{
    int v1[v1_size], v2[v2_size], v3[v3_size];
    int high1, high2, high3, low1, low2, low3;
    double cpu_time1, cpu_time2, cpu_time3;

    clock_t start1, start2, start3, stop1, stop2, stop3;
    
    srand(time(NULL));
   
    generate_random_vector(v1, v1_size);

    generate_random_vector(v2, v2_size);

    generate_random_vector(v3, v3_size);


    start1 = clock();
    high1 = linearHighest(v1, v1_size);
    low1 = linearLowest(v1, v1_size);
    stop1 = clock();
    cpu_time1 = ((double) (stop1 - start1) / (CLOCKS_PER_SEC)) * 1000;

    start2 = clock();
    high2 = linearHighest(v2, v2_size);
    low2 = linearLowest(v2, v2_size);
    stop2 = clock();
    cpu_time2 = ((double) (stop2 - start2) / (CLOCKS_PER_SEC)) * 1000;

    start3 = clock();
    high3 = linearHighest(v3, v3_size);
    low3 = linearLowest(v3, v3_size);
    stop3 = clock();
    cpu_time3 = ((double) (stop3 - start3) / (CLOCKS_PER_SEC)) * 1000;
    printf("linear time1: %f\nlinear time2: %f\nlinear time3: %f\n", cpu_time1, cpu_time2, cpu_time3);


    ThreadResolve(v1, v1_size, high1, low1);

    ThreadResolve(v2, v2_size, high2, low2);

    ThreadResolve(v3, v3_size, high3, low3);

    return 0;
}
