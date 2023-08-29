#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <mqueue.h>


#define BUF 200

int main(){
    char file_name[]= "/server.txt";
    char buf[BUF];
    mqd_t fd;
    struct mq_attr at;

    at.mq_flags = 0;
    at.mq_maxmsg = 5;
    at.mq_msgsize = BUF;
    at.mq_curmsgs = 0;

    printf("Server start\n");
    
    if((fd = mq_open(file_name, O_CREAT|O_RDWR, 0666, &at)) == -1){
        perror("mq_open");
        return -1;
    }

    printf("Waitin of the message...\n");

    if(mq_receive(fd, buf, BUF, NULL) == -1){
        perror("mq_receive");
        return -1;
    }
    printf("Message from the client - %s", buf);

    printf("Enter the message -");
    fgets(buf, BUF, stdin);
    if(mq_send(fd, buf, BUF, 1) == -1){
        perror("mq_send");
        return -1;
    }

    mq_close(fd);
    printf("Server end\n");
}