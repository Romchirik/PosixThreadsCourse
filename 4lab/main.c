#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#define ITERATIONS 10
#define tre
void *child_routine(void *arg)
{
    const char *words = "Hello from child thread\n";

    while (1)
    {
        printf("%s", words);
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

    sleep(2);

    err = pthread_cancel(nthread_id);
    if (err != 0)
    {
        printf("Unable to cancel child thread\n");
    }

    return 0;
}
