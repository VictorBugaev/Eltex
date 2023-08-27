#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/wait.h>
#include <unistd.h>


int main(){
    int fides[2];
    pid_t pid;
    char buf[200]="RUSSIA";
    int status;

    printf("Start p1\n");
    if(pipe(fides) == -1){
        perror("pipe");
        exit(1);
    }

    printf("Create pipe: read - %d, write - %d\n", fides[0], fides[1]);
    
    printf("Enter your message: ");
    fgets(buf, sizeof(buf), stdin);
    
    if((pid = fork()) == -1){
        perror("fork");
        exit(1);
    } else if (pid == 0){
        printf("My pid - %d\n", getpid());
        dup2(fides[0], 0);
        close(fides[0]);
        close(fides[1]);
        execl("./p2", "./p2", NULL);
        perror("execl");
        exit(1);
    }

    

    if(write(fides[1], buf, sizeof(buf)) == -1){
        perror("write");
        exit(1);
    }

    close(fides[0]);
    close(fides[1]);
    wait(&status);

    printf("End p1\n");

    return 0;
}