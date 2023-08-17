#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SIZE_BUF 200

int port;
char *addr;

void *therad_recvfrom(void *args){
    int fd = *(int *)args;
    int res;
    char buffer[SIZE_BUF];
    struct sockaddr_in server;
    int size = sizeof(server);
    struct pollfd fdp;
    fdp.fd = fd;
    fdp.events = POLLIN;
    while(1){
        res = poll(&fdp, 1, 100);
        if(res == -1)
            perror ("poll");
        else if(res > 0){
            if(recvfrom(fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&server, (socklen_t *)&size) == -1){
                perror("recvfrom");
                return -1;
            }
            printf("message - %s\n", buffer);
        }
    }
    return 0;
}

int main(int argc, char *argv[]){
    int fd;
    char buffer[SIZE_BUF];
    pthread_t id;
    int st;
    struct sockaddr_in client;
    if(argc < 3){
        printf("Not enough arguments - source addr, source port\n");
        return -1;
    }
    if(sscanf(argv[2], "%d", &port) != 1)
    {
        printf("Error: incorrect source addr %s\n", argv[2]);
        return -1;
    }
    if((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
        perror("socket");
        return -1;
    }
    printf("Socket - %d\n", fd);
    memset((char *)&client, 0, sizeof(struct sockaddr_in));
    if(inet_pton(AF_INET, argv[1], &client.sin_addr) <= 0){
        printf("Erorr - incorrect source addr %s\n", argv[1]);
        close(fd);
        return -1;
    }
    client.sin_port = htons(port);
    client.sin_family = AF_INET;

    if(bind(fd, (struct sockaddr *)&client, sizeof(struct sockaddr_in)) == -1){
        perror("bind");
        return -1;
    }
    pthread_create(&id, NULL, therad_recvfrom, (void **)&fd);
    while (1)
    {
        printf("Input - ");
        fgets(buffer, SIZE_BUF, stdin);
    }
    pthread_join(id, (void **)&st);
    close(fd);
    printf("End client\n");
    return 0;
}