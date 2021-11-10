#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUMBER_OF_PRINTS 10  //per each thread

sem_t* sem1;
sem_t* sem2;


void shutdown() {
    sem_unlink("/_sem1_mysem_");
    sem_unlink("/_sem2_mysem_");
}

void* routine(void* data) {
    for (int i = 0; i < 10; i++) {
        sem_wait(sem2);
        printf("Hello from child: %d\n", i);
        sem_post(sem1);
    }
}

int initialize() {
    sem1 = sem_open("/_sem1_mysem_", O_CREAT, 0600, 1);
    sem2 = sem_open("/_sem2_mysem_", O_CREAT, 0600, 0);
    if(sem1 == SEM_FAILED || sem2 == SEM_FAILED){
        return 1;
    } else {
        return 0;
    }
}

int main(int argc, char** argv) {
    if (initialize()) {
        perror("Unable to initialize, shutdown\n");
        shutdown();
        exit(EXIT_FAILURE);
    }

    routine(NULL);

    shutdown();
    return EXIT_SUCCESS;
}