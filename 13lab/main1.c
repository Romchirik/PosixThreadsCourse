
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUMBER_OF_PRINTS 10  //per each thread

pthread_mutex_t mutex;
pthread_cond_t condition;
volatile int turn = 0;

void shutdown() {
    if (pthread_mutex_destroy(&mutex)) {
        perror("Unable to destroy mutex\n");
    }
    if (pthread_cond_destroy(&condition)) {
        perror("Unable to destroy condition variable\n");
    }
}

int initialize() {
    if (pthread_mutex_init(&mutex, NULL)) {
        return EXIT_FAILURE;
    }
    if (pthread_cond_init(&condition, NULL)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void* child_routine2(void* data) {
    const int my_num = 2;
    const int next_num = 0;

    pthread_mutex_lock(&mutex);
    for (int i = 0; i < NUMBER_OF_PRINTS; i++) {
        while (turn != my_num) {
            pthread_cond_wait(&condition, &mutex);
        }

        printf("Hello from child 2: %d\n", i);
        turn = next_num;
        pthread_cond_broadcast(&condition);
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void* child_routine1(void* data) {
    const int my_num = 1;
    const int next_num = 2;

    pthread_mutex_lock(&mutex);
    for (int i = 0; i < NUMBER_OF_PRINTS; i++) {
        while (turn != my_num) {
            pthread_cond_wait(&condition, &mutex);
        }
        printf("Hello from child 1: %d\n", i);
        turn = next_num;
        pthread_cond_broadcast(&condition);
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void* parent_routine(void* data) {
    const int my_num = 0;
    const int next_num = 1;

    pthread_mutex_lock(&mutex);
    for (int i = 0; i < NUMBER_OF_PRINTS; i++) {
        while (turn != my_num) {
            pthread_cond_wait(&condition, &mutex);
        }
        printf("Hello from parent: %d\n", i);
        turn = next_num;
        pthread_cond_broadcast(&condition);
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main(int argc, char** argv) {
    if (initialize()) {
        perror("Unable to initialize, shutdown\n");
        shutdown();
        exit(EXIT_FAILURE);
    }
    pthread_t child_thread1, child_thread2;

    if (pthread_create(&child_thread1, NULL, child_routine1, NULL)) {
        perror("Unable to start child thread");
        shutdown();
    }

    usleep(100);
    if (pthread_create(&child_thread2, NULL, child_routine2, NULL)) {
        perror("Unable to start child thread");
        shutdown();
    }

    usleep(100);
    parent_routine(NULL);

    if (pthread_join(child_thread1, NULL) || pthread_join(child_thread2, NULL)) {
        perror("Unable to properly join child thread, shutdown\n");
        shutdown();
        return EXIT_FAILURE;
    }

    shutdown();
    return EXIT_SUCCESS;
}