#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>

void process1() {
    printf("Process1 pid = %d\n", getpid());

    pid_t pid4 = fork();
    if (pid4 == 0) {
        printf("Process4 pid = %d\n", getpid());
        exit(4);
    }

    pid_t pid5 = fork();
    if (pid5 == 0) {
        printf("Process5 pid = %d\n", getpid());
        exit(5);
    }

    int status4, status5;
    wait(&status4);
    printf("[2] status = %d\n", WEXITSTATUS(status4));
    wait(&status5);
    printf("[2] status = %d\n", WEXITSTATUS(status5));
    exit(2);
}

void process2() {
    printf("Process2 pid = %d\n", getpid());

    pid_t pid3 = fork();
    if (pid3 == 0) {
        printf("Process3 pid = %d\n", getpid());
        exit(6);
    }

    int status3;
    wait(&status3);
    printf("[3] status = %d\n", WEXITSTATUS(status3));
    exit(3);
}

int main(void) {
    printf("Parent pid = %d\n", getpid());

    pid_t pid1 = fork();
    if (pid1 == 0) {
        process1();
    }

    int status1;
    wait(&status1);
    printf("[1] status = %d\n", WEXITSTATUS(status1));

    pid_t pid2 = fork();
    if (pid2 == 0) {
        process2();
    }

    int status2;
    wait(&status2);
    printf("[1] status = %d\n", WEXITSTATUS(status2));

    printf("END\n");
    return 0;
}
