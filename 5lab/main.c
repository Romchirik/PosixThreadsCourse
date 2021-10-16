#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#define ITERATIONS 10


void cleanup_handler(void *arg)
{
    write(STDOUT_FILENO, "Cancelling threa1\n", 18);
}

void *child_routine(void *arg)
{
    const char *words = "Hello from child thread";
    pthread_cleanup_push(cleanup_handler, NULL);
    while (1)
    {
        pthread_testcancel();
        printf("%s\n", words);
    }

    pthread_cleanup_pop(1);
    return NULL;
}

int main(int argc, char *argv[])
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

    pthread_exit(NULL);
    return 0;
}
