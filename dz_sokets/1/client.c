#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <sys/socket.h>

#define PATH "SERVER"

int main (int argc, char *argv[]){
    int fd;
    char buf [100];
    char file[100];
    struct sockaddr server;
    if(argc > 1){
        strcpy(file,argv[1]);
    }
    else{
        strcpy(file,PATH);
    }
    fd = socket(AF_LOCAL,SOCK_STREAM,0);
    if (fd == -1){
        perror("socket");
        return -1;
    }
    server.sa_family = AF_LOCAL;
    strcpy(server.sa_data, file); 
    if (connect(fd, &server, sizeof(struct sockaddr)) == -1){
        perror("connect");
        return -1;
    }
    printf("Input: ");
    fgets(buf, sizeof(buf), stdin);
    if (send(fd, buf, sizeof(buf), 0) == 1){
        perror("send");
        return -1;
    }
    if(recv (fd, buf, sizeof(buf), 0) == -1){
        perror("recv");
        return -1;
    }
    printf("Server: %s\n", buf);
    if(close(fd) == -1){
        perror ("close");
        return -1;
    }
    printf("End\n");
    return 0;
}