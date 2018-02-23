#include <sys/shm.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SIZE 1024
extern int errno;

int main() {
    int shmid;
    char *shmaddr;
    struct shmid_ds buf;
    int flag = 0;
    int pid;
    shmid = shmget(IPC_PRIVATE, SIZE, IPC_CREAT | 0600);
    if (shmid < 0) {
        perror("get shm ipc_id error");
        return -1;
    }
    pid = fork();
    if (pid == 0) {
        shmaddr = (char *)shmat(shmid, NULL, 0);
        strcpy(shmaddr, "Hi, I am child process!\n");
        shmdt(shmaddr);
    } else if (pid > 0) {
        sleep(1);
        shmaddr = (char *)shmat(shmid, NULL, 0);
        printf("%s", shmaddr);
        shmdt( shmaddr ) ;
    } else {
        perror("fork error");
        shmctl(shmid, IPC_RMID, NULL);
    }
    return 0;
}