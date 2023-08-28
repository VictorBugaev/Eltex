#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "struct_msgbuf.h"

int main (){
    char file_name[]=".";
    key_t fd;
    int mq_key, len; 
    struct msgbuf msg;

    printf("Client start\n\n");

    if ((fd = ftok(file_name, 1)) == -1){
        perror("ftok");
        return -1;
    }

    if((mq_key = msgget(fd, IPC_CREAT)) == -1){
        perror("msget");
        return -1;
    }
    printf("Message queues key - %d\n\n", mq_key);

    len = sizeof(struct msgbuf) - sizeof(long);

    
    
    printf("Enter the message :");
    fgets(msg.mtext, BUF, stdin);
    msg.mtype = 1;
    
    if(msgsnd(mq_key, &msg, len, 0) == -1){
        perror("msgsnd");
        return -1;
    }
    printf("\nWaiting of message from server ...\n\n");
    if(msgrcv(mq_key, &msg, len, 2, 0) == -1){
        perror("msgrecv");
        return -1;
    }

    printf("Received messge - %s\n", msg.mtext);

    printf("Clietn end\n");
}