#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 4
void *child_routine(void *args)
{
    for (char **i = (char **)args; *i != NULL; i++)
    {
        printf("%s\n", *i);
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    char *lines1[] = {"I'm thread 1, this is line 1", "I'm thread 1, this is line 2", "I'm thread 1, this is line 3", "I'm thread 1, this is line 4", NULL};
    char *lines2[] = {"I'm thread 2, this is line 1", "I'm thread 2, this is line 2", NULL};
    char *lines3[] = {"I'm thread 3, this is line 1", NULL};
    char *lines4[] = {"I'm thread 4, this is line 1", "I'm thread 4, this is line 2", "I'm thread 4, this is line 3", NULL};

    pthread_t thread[NUM_THREADS];
    int err = 0;

    err |= pthread_create(&thread[0], NULL, child_routine, lines1);
    err |= pthread_create(&thread[1], NULL, child_routine, lines2);
    err |= pthread_create(&thread[2], NULL, child_routine, lines3);
    err |= pthread_create(&thread[3], NULL, child_routine, lines4);

    if (err != 0)
    {
        perror("unable to start one or more thread, terminating...");
        return 1;
    }

    pthread_exit(NULL);
    return 0;
}