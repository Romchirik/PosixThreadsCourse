#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFSIZE 1000
#define MAX_STRINGS 100
#define SLEEP_FACTOR 1000

void *sorting_node(void *arg) {
    char *string = arg;
    usleep(strlen(string) * SLEEP_FACTOR);
    printf("%s", string);
    return NULL;
}

int main(int argc, char **argv) {
    char **strings = (char **)calloc(MAX_STRINGS, sizeof(char *));
    char buffer[BUFSIZE];
    pthread_t threads[MAX_STRINGS];
    size_t lines_read = 0;
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
            perror("Unable to join done of threads\n");
        }
        free(strings[i]);
    }
    free(strings);
    pthread_exit(NULL);
    return 0;
}
