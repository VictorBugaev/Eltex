#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>

#define PATH "SERVER"

int main(){
    int fd;
    char buf[100];
    socklen_t client_size;
    struct sockaddr server;
    struct sockaddr client;
    server.sa_family = AF_LOCAL;
    strcpy(server.sa_data, PATH);
    fd = socket(AF_LOCAL,SOCK_DGRAM,0);
    if (fd == -1){
        perror("socket");
        return -1;
    }
    unlink(PATH);
    if (bind(fd,(struct sockaddr *)&server,sizeof(server))){
        perror("bind");
        return -1;
    }
    listen(fd, 1); 
    client_size = sizeof(client);
    if(recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr*)&client, &client_size) == -1){
        perror("recvfrom");
        return -1;
    }
    printf("Client - %s\n", buf);

    if (close(fd) == -1){
        perror("close");
        return -1;
    }
    remove (PATH);
    printf("End\n");
    return 0;
}
