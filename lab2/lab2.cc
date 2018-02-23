#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

const int N = 100;

sem_t sem_full;
sem_t sem_empty;
sem_t sem_count;
int a;
int i;

void *cal(void *arg) {
    while (i <= N) {
        sem_wait(&sem_empty);
        a += i;
        sem_post(&sem_full);
    }
    pthread_exit(NULL);
}

void *print(void *arg) {
    for (i = 0; i <= N; ++i) {
        sem_wait(&sem_full);
        printf("%d -> %d\n", i, a);
        sem_post(&sem_empty);
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t thread1;
    pthread_t thread2;

    sem_init(&sem_full, 0, 0);
    sem_init(&sem_empty, 0, 1);

    if ((pthread_create(&thread1, NULL, cal, NULL)) == -1) {
        perror("Create thread1 failed!\n");
    }

    if ((pthread_create(&thread2, NULL, print, NULL)) == -1) {
        perror("Create thread2 failed!\n");
    }

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    sem_destroy(&sem_full);
    sem_destroy(&sem_empty);

    return 0;
}