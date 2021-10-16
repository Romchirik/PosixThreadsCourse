#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <math.h>

#define NUM_STEPS (2000000000L)
#define NUM_THREADS 2
#define EXACT_STEPS (NUM_STEPS / NUM_THREADS) * (NUM_THREADS + 1)

double results[NUM_THREADS];

inline double __n_el(int n)
{
    if (n % 2 == 0)
    {
        return 1.0 / (n * 2.0 + 1.0);
    }
    else
    {
        return (-1.0) / (n * 2.0 + 1.0);
    }
}

void *parallel_counter_routine(void *arg)
{
    double tmp = 0;
    size_t shift = (size_t)arg;

    for (size_t i = 0; i < EXACT_STEPS; i++)
    {
        tmp += __n_el(shift + NUM_THREADS * i);
    }

    results[shift] = tmp;
    printf("thread %ld result %lf\n", shift, tmp);
    return NULL;
}

double sum(const double *arr, const size_t size)
{
    double tmp = 0;
    for (size_t i = 0; i < size; i++)
    {
        printf("%lf\n", arr[i]);
        tmp += arr[i];
    }
    return tmp;
}

int main(int argc, char **argv)
{
    pthread_t threads[NUM_THREADS];

    double pi = 0;

    for (size_t i = 0; i < NUM_THREADS; i++)
    {
        int err = pthread_create(&threads[i], NULL, parallel_counter_routine, (void *)i);
        if (err != 0)
        {
            perror("Unable to start one of threads, unable to continue, terminating\n");
            return 0;
        }
    }

    for (size_t i = 0; i < NUM_THREADS; i++)
    {
        int err = pthread_join(threads[i], NULL);
        if (err != 0)
        {
            perror("Unable to join done of threads, panic!\n");
        }
    }

    pi = 4 * sum(results, NUM_THREADS);
    printf("pi: %lf\n",  pi);
    printf("delta %.10lg\n", M_PI - pi);
    return (EXIT_SUCCESS);
}