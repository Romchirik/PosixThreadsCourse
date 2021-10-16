/* 
 * File:   din_phil.c
 * Author: nd159473 (Nickolay Dalmatov, Sun Microsystems)
 * adapted from http://developers.sun.com/sunstudio/downloads/ssx/tha/tha_using_deadlock.html
 *
 * Created on January 1, 1970, 9:53 AM
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/errno.h>
#include <unistd.h>

#define PHILO 5
#define DELAY 300
#define FOOD 50

#define true 1
#define false 0

pthread_mutex_t forks[PHILO];
pthread_t phils[PHILO];
void *philosopher(void *id);
int food_on_table();
char get_forks(int);
void down_forks(int, int);
pthread_mutex_t foodlock;

int sleep_seconds = 0;

int main(int argc, char **argv) {
    int i;

    if (argc == 2)
        sleep_seconds = atoi(argv[1]);

    pthread_mutex_init(&foodlock, NULL);
    for (i = 0; i < PHILO; i++) {
        pthread_mutex_init(&forks[i], NULL);
    }
    for (i = 0; i < PHILO; i++) {
        pthread_create(&phils[i], NULL, philosopher, (void *)i);
    }
    for (i = 0; i < PHILO; i++) {
        pthread_join(phils[i], NULL);
    }
    return 0;
}

void *philosopher(void *num) {
    int id;
    int left_fork, right_fork, f = FOOD;

    id = (int)num;
    printf("Philosopher %d sitting down to dinner.\n", id);
    right_fork = id % PHILO;
    left_fork = (id + 1) % PHILO;

    /* Wrap around the forks. */
    if (left_fork == PHILO)
        left_fork = 0;
    int a = 0;

    while (f) {
        // get_fork(id, right_fork, "right");
        // get_fork(id, left_fork, "left ");
        if (get_forks(id)) {
            printf("Philosopher %d: eating.\n", id);
            usleep(DELAY * (FOOD - f + 1));
            down_forks(left_fork, right_fork);
            f = food_on_table();
            a++;
            printf("Philosopher %d: get dish %d.\n", id, f);
        } else {
            printf("Philosopher %d not allowed to eat, waiting\n", id);
        }
        printf("Philosopher %d: thinking\n", id);
        usleep(DELAY * (FOOD - f + 1));
    }
    printf("Philosopher %d is done eating %d.\n", id, a);
    return (NULL);
}

int food_on_table() {
    static int food = FOOD;
    int myfood;

    pthread_mutex_lock(&foodlock);
    if (food > 0) {
        food--;
    }
    myfood = food;
    pthread_mutex_unlock(&foodlock);
    return myfood;
}

//todo fix id's
char get_forks(int id) {
    int left_fork = id % PHILO;
    int right_fork = (id + 1) % PHILO;

    pthread_mutex_lock(&forks[left_fork]);

    if (pthread_mutex_trylock(&forks[right_fork]) == EBUSY) {
        pthread_mutex_unlock(&forks[left_fork]);
        return false;
    } else {
        return true;
    }
}

void down_forks(int f1, int f2) {
    pthread_mutex_unlock(&forks[f1]);
    pthread_mutex_unlock(&forks[f2]);
}