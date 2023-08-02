#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>

#define PORT 4567
#define IP "127.0.0.1"

int main(){
    int fd;
    char buf[100];
    socklen_t client_size;
    struct sockaddr_in server;
    struct sockaddr_in client;
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
        perror ("socket");
        return -1;
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);
    if (bind(fd,(struct sockaddr *)&server, sizeof(server)) == -1){
        perror ("bind");
        return -1;
    }
    printf("Сервер запущен. Ожидание клиента...\n");
    
    client_size = sizeof(client);
    if(recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr *)&client, &client_size) == -1){
        perror("recvfrom");
        return -1;        
    }
    printf("Сообщение клиента - %s", buf);
    printf("Введите сообщение клиенту - ");
    fgets(buf, sizeof(buf), stdin);
    if(sendto(fd, buf, sizeof(buf), 0, (struct sockaddr *)&client,client_size) == -1){
    perror("sendto");
    return -1;
}
    if(close(fd) == -1){
        perror("close");
        return -1;
    }
    printf("End\n");
    return -1;
}

