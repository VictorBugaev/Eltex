#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(){
    char pipe_name[]="pipe";
    char buf[200];
    int fd, status;
    

    printf("\tP2 - start\n");
    printf("\tMy pid - %d\n", getpid());
    fd = open(pipe_name, O_WRONLY | O_NONBLOCK);
    
    if( fd == -1 ){
        perror("open");
        return -1;
    }

    printf("\tEnter the message: ");
    fgets(buf, sizeof(buf), stdin);

    if(write(fd, buf, sizeof(buf)) == -1){
        perror("write");
        return -1;
    }
    
    if(close(fd) == -1){
            perror("close");
            return -1;
        }
    printf("\tP2 - end\n");
    return 0;
}
