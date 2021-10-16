
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUMBER_OF_PRINTS 10  //per each thread

pthread_mutex_t mutex;
pthread_cond_t condition;
volatile int turn = 1;
//emergency shutdown
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

void* child_routine(void* data) {
    int is_first = 0;
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < NUMBER_OF_PRINTS; i++) {
        while (is_first != turn) {
            pthread_cond_wait(&condition, &mutex);
        }
        printf("Hello from child: %d\n", i);
        turn = !turn;
        pthread_cond_signal(&condition);
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void* parent_routine(void* data) {
    int is_first = 1;
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < NUMBER_OF_PRINTS; i++) {
        while (is_first != turn) {
            pthread_cond_wait(&condition, &mutex);
        }
        printf("Hello from parent: %d\n", i);
        turn = !turn;
        pthread_cond_signal(&condition);
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
    pthread_t child_thread;

    if (pthread_create(&child_thread, NULL, child_routine, NULL)) {
        perror("Unable to start child thread");
        shutdown();
    }
    parent_routine(NULL);

    if (pthread_join(child_thread, NULL)) {
        perror("Unable to properly join child thread, shutdown\n");
        shutdown();
        return EXIT_FAILURE;
    }

    shutdown();
    return EXIT_SUCCESS;
}