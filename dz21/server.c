#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "struct_msgbuf.h"

int main(){
    char file_name[]=".";
    key_t fd;
    int mq_key, len; 
    struct msgbuf msg;

    printf("Server start\n\n");

    if ((fd = ftok(file_name, 1)) == -1){
        perror("ftok");
        return -1;
    }

    if((mq_key = msgget(fd, IPC_CREAT | 0660)) == -1){
        perror("msget");
        return -1;
    }
    printf("Message queues key - %d\n\n", mq_key);
    
    
    len = sizeof(struct msgbuf) - sizeof(long);
    printf("Waiting of message from client ...\n\n");
    if(msgrcv(mq_key, &msg, len, 1, 0) == -1){
        perror("msgrcv");
        return -1;
    }
    printf("Received message - %s\n", msg.mtext);


    msg.mtype = 2;
    printf("Enter the message -");
    fgets(msg.mtext, BUF, stdin);
    if(msgsnd(mq_key, &msg, len, 0) == -1){
        perror("msgsnd");
        return -1;
    }

    printf("\nServer end\n");
    return 0;
}