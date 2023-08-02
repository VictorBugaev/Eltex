#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define PATH "SERVER"

void replace_enter(){

}

int main(){
    int fd, new_fd;
    socklen_t client_size;
    char buf[100];
    struct sockaddr server;
    struct sockaddr client;
    server.sa_family = AF_LOCAL;
    strcpy(server.sa_data, PATH);
    fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (fd==-1){
        perror("socket");
        return -1;
    }
    if(bind(fd,(struct sockaddr *)&server, sizeof(server))==1){
        perror("bind");
        return -1;
    }
    listen(fd,1);
    client_size = sizeof(client);
    new_fd = accept(fd, (struct sockaddr *)&client, &client_size);
    if (new_fd==-1){
        perror("accept");
        return -1;
    }
    if (recv(new_fd,buf,sizeof(buf),0) == -1){
        perror("recv");
        return -1;
    }    
    printf("Client - %s\nInput:", buf);
    fgets(buf,sizeof(buf),stdin);
    if(send(new_fd ,buf ,sizeof(buf) ,0) == -1){
        perror("send");
        return -1;
    }
    close(new_fd);
    close(fd);
    remove(PATH);
    printf("End\n");
    return 0;
}
