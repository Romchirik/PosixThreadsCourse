#include <stdio.h>
#include <pthread.h>
#include <string.h>

#define ITERATIONS 10

void *parent_roitine(void *arg)
{
    const char *words = "Hello from parent thread\n";
    for (int i = 0; i < ITERATIONS; i++)
    {
        printf("%d: %s", i + 1, words);
    }
    
}

void *child_routine(void* arg)
{
    const char *words = "Hello from child thread\n";

    for (int i = 0; i < ITERATIONS; i++)
    {
        printf("%d: %s", i + 1, words);
    }
}

int main()
{
    pthread_t nthread_id;

    int err = pthread_create(&nthread_id, NULL, child_routine, NULL);
    if (err != 0)
    {
        printf("Unable to create new thread, error: %s", strerror(err));
        return 1;
    }
    
    
    parent_roitine(NULL);
    return 0;
}
