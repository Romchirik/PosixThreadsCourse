#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <signal.h>
#include <unistd.h>

#define NUM_THREADS 10
#define STOP_CHECK 10000
double results[NUM_THREADS];
volatile char if_exit = 0;
volatile size_t max_iters = 0;
pthread_barrier_t barrier;
pthread_mutex_t sync_mutex;

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

    for (size_t i = 0;; i++)
    {
        tmp += __n_el(shift + NUM_THREADS * i);
        if (i % STOP_CHECK == 0)
        {
            if (if_exit)
            {
                //типы мутексов //fast, recursive, error-checking, adaptive

                //область действия //process-private || system-wide

                //параметры мутексов

                // PTHREAD_MUTEX_STALLED_NP  || PTHREAD_MUTEX_ROBUST_NP (EOWNERDEAD) - what to do on terminate
                // SHARED || PROCESS_PRIVATE
                // protocol: PTHREAD_PRIO_NONE || PTHREAD_PRIO_INHERIT (can change processes priority) || PTHREAD_PRIO_PROTECT (protects owner thread priority)
                // 
                // 
                // 

                //операции над мутексами
                
                pthread_mutex_lock(&sync_mutex);
                if (max_iters < i)
                {
                    max_iters = i;
                }
                pthread_mutex_unlock(&sync_mutex);

                pthread_barrier_wait(&barrier);

                if (max_iters != i)
                {
                    while (i != max_iters)
                    {
                        tmp += __n_el(shift + NUM_THREADS * i);
                        i++;
                    }
                }

                results[shift] = tmp;
                printf("Thread %ld: %ld iterations\n", shift, i);
                pthread_exit(EXIT_SUCCESS);
            }
        }
    }
}

double sum(const double *arr, const size_t size)
{
    double tmp = 0;
    for (size_t i = 0; i < size; i++)
    {
        tmp += arr[i];
    }
    return tmp;
}

static void sig_usr(int signo)
{
    if_exit = 1;
}

int main(int argc, char **argv)
{
    pthread_t threads[NUM_THREADS];

    double pi = 0;

    pthread_barrier_init(&barrier, NULL, NUM_THREADS);

    if (sigset(SIGINT, sig_usr) == SIG_ERR)
    {
        perror("failed to handle SIGINT");
        return 0;
    }

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

    printf("pi: %lf\n", 4 * sum(results, NUM_THREADS));
    pthread_mutex_destroy(&sync_mutex);
    return (EXIT_SUCCESS);
}