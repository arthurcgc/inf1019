#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define v1_size 1000
#define v2_size 4000
#define v3_size 16000

typedef struct args
{
    pthread_t tid;
    int cand;
    int *v;
} Args;

int highest = 0;

void *getHighest(void *p)
{
    Args *temp = (Args*) p;
    
    for(int i = 0; i < 100; i++)
    {
        temp->cand = *temp->v;
        if(temp->cand > highest)
            highest = temp->cand;
        temp->v++;
    }
    pthread_exit(NULL);
}

void generate_random_vector(int *v)
{
    for(int i = 0; i < v1_size; i++)
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
        temp->v = v + 100 * i;
        pthread_create(&threads[i], NULL, (void*) getHighest , (void*) temp);
    }
    for (int i = 0; i < threads_num; i++)
        pthread_join(threads[i], NULL);
}

void PrintResults(int *v, int size)
{

}

int main(int argc, char const *argv[])
{
    int v1[v1_size], v2[v2_size], v3[v3_size];
    int resp1, resp2, resp3;
    clock_t start, end;
    double cpu_time_ms;

    srand(time(NULL));
    generate_random_vector(v1);
    generate_random_vector(v2);
    generate_random_vector(v3);

    resp1 = linearHighest(v1, v1_size);
    resp2 = linearHighest(v2, v2_size);
    resp3 = linearHighest(v3, v3_size);

    start = clock();
    solveWithThreads(v1, v1_size);
    end = clock();
    cpu_time_ms = ((double) (end - start) / (CLOCKS_PER_SEC)) * 1000;
    printf("\nv1 resposta sem threads: %d\n", resp1);
    printf("v1 resposta com threads: %d\n", highest);
    if(resp1 == highest)
        printf("numeros iguais\n\nTempo de execução: %f ms\n", cpu_time_ms);
    highest = 0;

    printf("----------------------------------------");
    start = clock();
    solveWithThreads(v2, v2_size);
    end = clock();
    cpu_time_ms = ((double) (end - start) / (CLOCKS_PER_SEC)) * 1000;
    printf("\nv2 resposta sem threads: %d\n", resp2);
    printf("v2 resposta com threads: %d\n", highest);
    if(resp2 == highest)
        printf("numeros iguais\n\nTempo de execução: %f ms\n", cpu_time_ms);
    highest = 0;

    printf("----------------------------------------");
    start = clock();
    solveWithThreads(v3, v3_size);
    end = clock();
    cpu_time_ms = ((double) (end - start) / (CLOCKS_PER_SEC)) * 1000;
    printf("\nv3 resposta sem threads: %d\n", resp3);
    printf("v2 resposta com threads: %d\n", highest);
    if(resp3 == highest)
        printf("numeros iguais\n\nTempo de execução: %f ms\n", cpu_time_ms);
    highest = 0;
    printf("----------------------------------------\n");

    return 0;
}
