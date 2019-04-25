#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int i = 0;

void *inc1(void *thread_id)
{
    for(int j = 0; j < 7; j++)
    {
        i++;
        printf("%d\n", i);
    }
    pthread_exit(NULL);
}

void *inc7(void *thread_id)
{
    for(int j = 0; j < 7; j++)
    {
        i+=7;
        printf("%d\n", i);
    }
    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
    pthread_t t1 = 1, t2 = 2;

    pthread_create(&t1, NULL, inc1, (void*) t1);
    pthread_create(&t2, NULL, inc7, (void*) t2);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}
