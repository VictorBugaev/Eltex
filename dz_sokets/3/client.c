#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 4567
#define IP "127.0.0.1"

int main(){
    int fd;
    char buf[100];
    char mes[100];
    struct sockaddr_in server;
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("socket");
        return -1;
    }
    memset(&server, '0', sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    if (inet_pton(AF_INET, IP, &server.sin_addr) <= 0){
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }
    if (connect(fd, (struct sockaddr *)&server, sizeof(server)) == -1){
       perror("connect");
       return -1; 
    }
    printf("Соединение установлено\n");
    printf("Введите сообщение: ");
    fgets(mes, sizeof(mes), stdin);

    if (send(fd, mes, strlen(mes), 0) < 0) {
        perror("send");
        return -1;
    }
    if (recv(fd, buf, sizeof(buf), 0) < 0) {
        perror("recv");
        return -1;
    }
    printf("Полученный ответ от сервера: %s\n", buf);
    close(fd);
    return 0;
}