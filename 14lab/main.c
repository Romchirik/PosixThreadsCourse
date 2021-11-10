#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUMBER_OF_PRINTS 10  //per each thread

sem_t sem1;
sem_t sem2;


void shutdown() {
    sem_destroy(&sem1);
    sem_destroy(&sem2);
}

void* child_routine(void* data) {
    for (int i = 0; i < 10; i++) {
        sem_wait(&sem2);
        printf("Hello from child: %d\n", i);
        sem_post(&sem1);
    }
}

void* parent_routine(void* data) {
    for (int i = 0; i < 10; i++) {
        sem_wait(&sem1);
        printf("Hello from parent: %d\n", i);
        sem_post(&sem2);
    }
}

int initialize() {
    if (sem_init(&sem1, 0, 1) || sem_init(&sem2, 0, 0)) {
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

//типы семафоров //

//область действия //process-private || system-wide

//параметры семафоров

// 
// 
// 
//
//
//

//операции над семафорами
// ENIVAL - невалиддный семафор
// 
// sem_init || sem_open
// sem_destroy || sem_close
// sem_wait() && sem_trywait()
// EINTR - прервана сигналом
// ENOSYS - не поддарживается систеой
// EAGAIN - невозможно моментально уменьшить значение (sem_trywait)
//  
// sem_post()
// ENOSYS - не поддерживается системой
// EOVERFLOW - семафор переполнен