#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SIZEBUF 200

int main(int argc, char *argv[]){
    int d_port;
    if(argc < 3){
        printf("Не хватает аргументов - dest addr, dest port\n");
        return -1;
    }
    if(sscanf(argv[2], "%d", &d_port) != 1){
        printf("Ошибка - неккорекнтый dest port %s\n", argv[2]);
        return -1;
    }
        if(d_port < 1025 || d_port > 0xFFFF){
        printf("Error port");
        return -1;
    }
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0){
        perror("socket");
        return -1;
    }
    int flag = 1;
    if(setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &flag, sizeof(flag))){
        perror("setsockopt");
        close(fd);
        return -1;
    }
    char buffer[SIZEBUF];
    struct sockaddr_in client;
    memset((void*)&client, 0, sizeof(client));
    client.sin_port = htons(d_port);
    client.sin_family = AF_INET;
    if(inet_pton(AF_INET, argv[1], &client.sin_addr) <= 0){
        printf("Ошибка неккорекный -dest address %s\n", argv[1]);
        close(fd);
        return -1;
    }
    while(1){
        printf("Input: ");
        memset(buffer, 0, SIZEBUF);
        fgets(buffer, SIZEBUF, stdin);
        replace_enter(buffer);
        if(!strcmp(buffer, "exit")){
            break;
        }
        if(sendto(fd, buffer, SIZEBUF, 0, (struct sockaddr*)&client, sizeof(client)) == -1){
            perror("sendto");
            break;
        }

    }
    if(close(fd)){
        perror("close");
    }

    printf("End server\n");
    return 0;
}
