#include <stdio.h>
#include <pthread.h>
#include <unistd.h>


void *countUp(void *thread_id)
{
    for(int i = 1; i < 21; i++)
    {
        printf("%d ", i);
        fflush(stdout);
        sleep(1);
    }
    printf("\n");
    pthread_exit(NULL);
}

void *countDown(void *threadid)
{
    for(int i = 30; i > 0; i--)
    {
        printf("%d ", i);
        fflush(stdout);
        sleep(3);
    }
    printf("\n");
    pthread_exit(NULL);
}


int main(int argc, char const *argv[])
{
    pthread_t t1 = 1, t2 = 2;

    pthread_create(&t1, NULL, countUp, (void*) t1);
    pthread_create(&t2, NULL, countDown, (void*) t2);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    return 0;
}
