#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 5

void *PrintHello(void *thread_id)
{
    printf("\n%d Hello World!\n", (int*) thread_id);
    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
    pthread_t threads[NUM_THREADS];
    int t;
    for(t = 0; t < NUM_THREADS; t++)
    {
        printf("Creating thread %d\n", t);
        pthread_create(&threads[t], NULL, PrintHello, (void*)t);
    }

    for(t = 0; t < NUM_THREADS; t++)
        pthread_join(threads[t], NULL);

    return 0;
}
