#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUMBER_OF_PRINTS 10  //per each thread

pthread_mutex_t mutexes[3];
pthread_cond_t condition;

//emergency shutdown
void shutdown() {
    if (pthread_mutex_destroy(&mutexes[0]) ||
        pthread_mutex_destroy(&mutexes[1]) ||
        pthread_mutex_destroy(&mutexes[2])) {
        perror("Unable to shutdown properly\n");
    }
}

void* child_routine(void* data) {
    pthread_mutex_lock(&mutexes[2]);
    for (int i = 0; i < NUMBER_OF_PRINTS; i++) {
        pthread_mutex_lock(&mutexes[1]);
        printf("Hello from child: %d\n", i);
        pthread_mutex_unlock(&mutexes[2]);
        pthread_mutex_lock(&mutexes[0]);
        pthread_mutex_unlock(&mutexes[1]);
        pthread_mutex_lock(&mutexes[2]);
        pthread_mutex_unlock(&mutexes[0]);
    }
    pthread_mutex_unlock(&mutexes[2]);
}

void* parent_routine(void* data) {
    for (int i = 0; i < NUMBER_OF_PRINTS; i++) {
        printf("Hello from parent: %d\n", i);
        pthread_mutex_lock(&mutexes[0]);
        pthread_mutex_unlock(&mutexes[1]);
        pthread_mutex_lock(&mutexes[2]);
        pthread_mutex_unlock(&mutexes[0]);
        pthread_mutex_lock(&mutexes[1]);
        pthread_mutex_unlock(&mutexes[2]);
    }
}

int initialize() {
    if (pthread_mutex_init(&mutexes[0], NULL) ||
        pthread_mutex_init(&mutexes[1], NULL) ||
        pthread_mutex_init(&mutexes[2], NULL) ||
        pthread_cond_init(&condition, NULL)) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int main(int argc, char** argv) {
    if (initialize()) {
        perror("Unable to initialize, shutdown\n");
        shutdown();
        exit(EXIT_FAILURE);
    }

    pthread_t child_thread;
    
    pthread_mutex_lock(&mutexes[1]);
    if (pthread_create(&child_thread, NULL, child_routine, NULL)) {
        perror("Unable to start child thread");
        shutdown();
    }
    usleep(100);
    parent_routine(NULL);
    pthread_mutex_unlock(&mutexes[1]);

    if (pthread_join(child_thread, NULL)) {
        perror("Unable to properly join child thread, shutdown\n");
        shutdown();
        return EXIT_FAILURE;
    }

    shutdown();
    return EXIT_SUCCESS;
}