#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(){
    char pipe_name[]="pipe";
    char buf[200];
    int fd, status;
    pid_t pid;

    printf("P1 - start\n");
    
    mkfifo(pipe_name, S_IRWXU);
    fd = open(pipe_name, O_RDONLY | O_NONBLOCK);
    if(fd == -1){
        perror("open");
        return -1;
    }
    
    pid = fork();
    if(pid == 0){
        printf("My pid - %d\n", getpid());
        execl("./p2", "./p2", NULL);
        printf("Error P2 start\n");
        if(close(fd) == -1){
            perror("close");
            return -1;
        }
    }
    sleep(2);
    if(read(fd, buf, sizeof(buf)) == -1){
        perror("read");
        return -1;
    }
    
    printf("message - %s", buf);
    wait(&status);
    
    if(close(fd) == -1){
            perror("close");
            return -1;
        }
    
    printf("P1 - end\n");
    return 0;
}
