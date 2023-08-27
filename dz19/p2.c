#include <stdio.h>
#include <pthread.h>
#include <sys/wait.h>
#include <unistd.h>

int main(){
    char buf[200];
    int read_size;

    printf("\tStart p2\n");
    printf("\tMy pid - %d\n", getpid());
    
    if(read(0, buf, sizeof(buf)) > 0){
        printf("\tread: %s", buf);
    }

    else{
        perror("read");
    }
    printf("\tEndp2\n");
    return 0;
}