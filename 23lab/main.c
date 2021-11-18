#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFSIZE 1000
#define MAX_STRINGS 100
#define SLEEP_FACTOR 1000

pthread_mutex_t mutex;

typedef struct __list_t {
    char *data;
    struct __list_t *next;
} list_t;

list_t *head = NULL;

void insert_node(list_t *head, list_t *node) {
    list_t *last = head;
    while (last->next != NULL) {
        last = last->next;
    }
    last->next = node;
}

void print_list(list_t *head) {
    list_t *last = head;
    while (last != NULL) {
        printf("%s", last->data);
        last = last->next;
    }
}
list_t *create_node() {
    list_t *tmp = calloc(1, sizeof(list_t));
    tmp->data = NULL;
    tmp->next = NULL;
    return tmp;
}

void list_free(list_t *head) {
    if (head != NULL) {
        list_free(head->next);
        free(head);
    }
}

void *sorting_node(void *arg) {
    char *string = arg;
    usleep(strlen(string) * SLEEP_FACTOR);
    pthread_mutex_lock(&mutex);
    if (head == NULL) {
        head = create_node();
        head->data = string;
    } else {
        list_t *tmp = create_node();
        tmp->data = string;
        insert_node(head, tmp);
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main(int argc, char **argv) {
    char **strings = (char **)calloc(MAX_STRINGS, sizeof(char *));
    char buffer[BUFSIZE];
    pthread_t threads[MAX_STRINGS];
    size_t lines_read = 0;

    if (pthread_mutex_init(&mutex, NULL) != 0) {
        perror("Unable to initialize mutex");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < MAX_STRINGS && buffer[0] != '\n'; i++) {
        fgets(buffer, BUFSIZE, stdin);
        strings[i] = (char *)malloc(strlen(buffer) * sizeof(char));
        strcpy(strings[i], buffer);
        lines_read++;
    }

    for (size_t i = 0; i < lines_read - 1; i++) {
        int err = pthread_create(&threads[i], NULL, sorting_node, strings[i]);
        if (err != 0) {
            perror("Unable to start new thread\n");
        }
    }

    for (size_t i = 0; i < lines_read - 1; i++) {
        int err = pthread_join(threads[i], NULL);
        if (err != 0) {
            perror("Unable to join one of threads\n");
        }
    }

    if (pthread_mutex_destroy(&mutex) != 0) {
        perror("Unable to properly close mutex");
    }

    printf("\n");
    print_list(head);
    for (size_t i = 0; i < lines_read; i++) {
        free(strings[i]);
    }
    free(strings);
    list_free(head);
    pthread_exit(NULL);
    return 0;
}
