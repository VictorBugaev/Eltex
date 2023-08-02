#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 4567

int main(){
    int fd, new_fd;
    struct sockaddr_in server;
    socklen_t socket_size;
    char buf [100];
    if ((fd = socket(AF_INET,SOCK_STREAM,0)) == -1){
        perror("socket");
        return -1;
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr  = INADDR_ANY;
    server.sin_port = htons(PORT);
    if (bind(fd, (struct sockaddr *)&server, sizeof(server)) == -1){
        perror("bind");
        return -1;
    }
    if (listen(fd,1) == -1){
        perror("listen");
        return -1;
    }
    printf("Ожидание подключения...\n");
    if ((new_fd = accept(fd, (struct sockaddr *)&server, &socket_size)) == -1){
        perror("accept");
        return -1;
    }
    if (recv(new_fd,buf,sizeof(buf),0) == -1){
        perror("recv");
        return -1;
    }       
    printf("Сообщение от клиента - %s\nВведите сообщение клиенту:\n", buf);
    fgets(buf,sizeof(buf),stdin);
    if(send(new_fd ,buf ,sizeof(buf) ,0) == -1){
        perror("send");
        return -1;
    }
    close(new_fd);
    close(fd);
    printf("End\n");
    return 0;
}
