#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <semaphore.h>

const int BLOCK_SIZE = 16;
const int BLOCK_NUM = 8;

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("src and dest expected!");
        return 0;
    }

    char *buf;

    int shmid_buf = shmget(IPC_PRIVATE, BLOCK_SIZE * BLOCK_NUM, IPC_CREAT | 0600 );
    int shmid_sem_empty = shmget(IPC_PRIVATE, sizeof(sem_t), IPC_CREAT | 0600 );
    int shmid_sem_full = shmget(IPC_PRIVATE, sizeof(sem_t), IPC_CREAT | 0600 );

    sem_t *sem_empty = (sem_t *)shmat(shmid_sem_empty, NULL, 0);
    sem_t *sem_full = (sem_t *)shmat(shmid_sem_full, NULL, 0);

    sem_init(sem_empty, 1, BLOCK_NUM);
    sem_init(sem_full, 1, 0);

    pid_t pid = fork();

    if (pid == -1) {
        perror("create subprocess failed");
        shmctl(shmid_buf, IPC_RMID, NULL);
        shmctl(shmid_sem_full, IPC_RMID, NULL);
        shmctl(shmid_sem_empty, IPC_RMID, NULL);
    } else if (pid == 0) {
        sem_empty = (sem_t *)shmat(shmid_sem_empty, NULL, 0);
        sem_full = (sem_t *)shmat(shmid_sem_full, NULL, 0);
        buf = (char *)shmat(shmid_buf, NULL, 0);

        FILE *fp = fopen(argv[1], "rb");
        for (int cur = 0;; cur = (cur + 1) % BLOCK_NUM) {
            sem_wait(sem_empty);
            int read = fread(buf + cur * BLOCK_SIZE + 1, 1, BLOCK_SIZE - 1, fp);
            printf("file read size: %d\n", read);
            buf[cur * BLOCK_SIZE] = read;
            sem_post(sem_full);
            if (read == 0) {
                break;
            }
        }
        fclose(fp);
        shmdt(buf);
    } else {
        sem_empty = (sem_t *)shmat(shmid_sem_empty, NULL, 0);
        sem_full = (sem_t *)shmat(shmid_sem_full, NULL, 0);
        buf = (char *)shmat(shmid_buf, NULL, 0);
        
        FILE *fp = fopen(argv[2], "wb");
        for (int cur = 0;; cur = (cur + 1) % BLOCK_NUM) {
            sem_wait(sem_full);
            int read = buf[cur * BLOCK_SIZE];
            sem_post(sem_empty);
            if (read == 0) {
                break;
            }
            fwrite(buf + cur * BLOCK_SIZE + 1, 1, read, fp);
            printf("file write size: %d\n", read);
        }
        fclose(fp);
        shmdt(buf);
        shmctl(shmid_buf, IPC_RMID, NULL);
        shmctl(shmid_sem_full, IPC_RMID, NULL);
        shmctl(shmid_sem_empty, IPC_RMID, NULL);
    }
    sem_destroy(sem_empty);
    sem_destroy(sem_full);
    return 0;
}