#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

int main (){
    printf("p1 Start\n");
    int fildes[2];
    int status;
    pipe(fildes);
    printf("read - %d, write - %d\n", fildes[0],fildes[1]);
    pid_t p;
    p = fork();
    if (p == 0){
        printf("My PID - %d\n", getpid());
        dup2(fildes[0],0);
        execl("./p2","./p2",NULL);
        printf("Error p2 start!\n");
        exit (0);
    }
    char test[256]="TEST";
    write(fildes[1],test,strlen(test));
    wait(&status);
    close(fildes[0]);
    close(fildes[1]);
    printf("p1 End\n");
}
