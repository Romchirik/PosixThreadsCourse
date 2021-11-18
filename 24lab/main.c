#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>

#define WORK_TIME 6000
sem_t a_detail_sem;
sem_t b_detail_sem;
sem_t c_detail_sem;
sem_t module_sem;

volatile char flag = 1;

void* create_a(void* arg) {
    while (flag) {
        sleep(1);
        sem_post(&a_detail_sem);
        printf("Detail A created!\n");
    }
    return NULL;
}

void* create_b(void* arg) {
    while (flag) {
        sleep(2);
        sem_post(&b_detail_sem);
        printf("Detail B created!\n");
    }
}

void* create_c(void* arg) {
    while (flag) {
        sleep(3);
        sem_post(&c_detail_sem);
        printf("Detail C created!\n");
    }
    return NULL;
}

void* create_module(void* arg) {
    while (flag) {
        sem_wait(&a_detail_sem);
        sem_wait(&b_detail_sem);
        sem_post(&module_sem);
        printf("Module created!\n");
    }
    return NULL;
}

void* createWidget() {
    while (flag) {
        sem_wait(&module_sem);
        sem_wait(&c_detail_sem);
        printf("Widget created!\n");
    }
    return NULL;
}

void shutdown() {
    if (sem_destroy(&a_detail_sem) ||
        sem_destroy(&a_detail_sem) ||
        sem_destroy(&a_detail_sem) ||
        sem_destroy(&a_detail_sem)) {
        perror("Unable to properly close semaphores");
    }
}

int main() {
    pthread_t a_thread;
    pthread_t b_thread;
    pthread_t c_thread;
    pthread_t module_thread;
    pthread_t widget_thread;

    if (sem_init(&a_detail_sem, 0, 0) ||
        sem_init(&b_detail_sem, 0, 0) ||
        sem_init(&c_detail_sem, 0, 0) ||
        sem_init(&module_sem, 0, 0)) {
        perror("Unable to initialize");
    }

    pthread_create(&a_thread, NULL, create_a, NULL);
    pthread_create(&b_thread, NULL, create_b, NULL);
    pthread_create(&c_thread, NULL, create_c, NULL);
    pthread_create(&module_thread, NULL, create_module, NULL);
    pthread_create(&widget_thread, NULL, createWidget, NULL);

    char stub;
    scanf("%c", &stub);

    if (pthread_cancel(widget_thread) ||
        pthread_cancel(module_thread) ||
        pthread_cancel(a_thread) ||
        pthread_cancel(b_thread) ||
        pthread_cancel(c_thread)) {
        perror("Unable to properly cancel one of threads");
    }

    shutdown();
    return 0;
}