#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#define ITERATIONS 10

void *parent_roitine(void *arg)
{

    const char *words = "Hello from parent thread\n";
    for (int i = 0; i < ITERATIONS; i++)
    {
        printf("%d: %s", i + 1, words);
    }
}

void *child_routine(void *arg)
{
    const char *words = "Hello from child thread\n";

    for (int i = 0; i < ITERATIONS; i++)
    {
        printf("%d: %s", i + 1, words);
    }
    pthread_exit((void *)2);
}

int main()
{
    pthread_t nthread_id;
    void *retval = NULL;
    int err = pthread_create(&nthread_id, NULL, child_routine, NULL);
    if (err != 0)
    {
        printf("Unable to create new thread, error: %s", strerror(err));
        return 1;
    }
    parent_roitine(NULL);
    err = pthread_join(nthread_id, &retval);

    if (err != 0)
    {
        printf("Unable to join child thread\n");
    }
    printf("retval: %ld\n", (long)retval);

    return 0;
}
