#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <wait.h>
#include <sys/types.h>

pid_t pid[2];
int fd[2];

void killSubprocess(int signum) {
    kill(pid[0], SIGUSR1);
    kill(pid[1], SIGUSR2);
}

void subprocess1() {
    signal(SIGINT, SIG_IGN);
    signal(SIGUSR1, [](int signum) {
        printf("Child Process 1 is Killed by Parent!\n");
        exit(0);
    });
    close(fd[0]);
    char buf[20];
    for (int i = 1;; ++i) {
        sprintf(buf, "I send you %d times", i);
        write(fd[1], buf, sizeof(buf));
        sleep(1);
    }
    close(fd[1]);
}

void subprocess2() {
    signal(SIGINT, SIG_IGN);
    signal(SIGUSR2, [](int signum) {
        printf("Child Process 2 is Killed by Parent!\n");
        exit(0);
    });
    close(fd[1]);
    char buf[20];
    while (read(fd[0], buf, sizeof(buf))) {
        printf("%s\n", buf);
    }
    close(fd[0]);
}

int main() {
    signal(SIGINT, killSubprocess);
    
    int p = pipe(fd);
    if (p == -1) {
        printf("Failed to create pipe\n");
        exit(EXIT_FAILURE);
    }

    pid[0] = fork();
    if (pid[0] == -1) {
        printf("Failed to fork subprocess1\n");
        exit(EXIT_FAILURE);
    }

    if (pid[0] == 0) {
        subprocess1();
    }

    pid[1] = fork();
    if (pid[1] == -1) {
        printf("Failed to fork subprocess2\n");
        exit(EXIT_FAILURE);
    }

    if (pid[1] == 0) {
        subprocess2();
    }

    wait(NULL);
    wait(NULL);
    close(fd[0]);
    close(fd[1]);
    printf("Parent Process is Killed!");

    return 0;
}