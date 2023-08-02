#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define IP "127.0.0.1"
#define PORT 4567

int main(){
    int fd;
    char buf[100];
    socklen_t client_size;
    struct sockaddr_in server;
    client_size = sizeof(server);
    if((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
        perror ("socket");
        return -1;
    }
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    if  (inet_pton(AF_INET, IP, &server.sin_addr) <= 0){
        perror("inet_pton");
        return -1;
    }
    printf("Введите сообщение - ");
    fgets(buf, sizeof(buf),stdin);
    if (sendto(fd, buf, sizeof(buf), 0, (struct sockaddr *)&server, sizeof(server)) == -1){
        perror("sendto");
        return -1;
    }
    if(recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr *)&server, &client_size) == -1){
        perror("recvfrom");
        return -1;
    }
    printf("Полученный ответ - %s\n", buf);
    if (close(fd) == -1){
        perror("close");
        return -1;
    }
    printf("End\n");
    return 0;
}
