#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SIZE_BUF 200

int port;

enum add{
    LOCADDR = 1,
    MULTIADDR,
    PORT,
};

void *thread_recvfrom(int *argc){
    int fd = *(int*)argc;
    int res;
    char buffer[SIZE_BUF];
    struct pollfd fdp;
    fdp.events = POLLIN;
    fdp.fd = fd;
    while(1){
        if((res = poll(&fdp, 1, 100)) == -1){
            perror("poll");
        }
        else if(res > 0){
            if(recv(fd, buffer, SIZE_BUF, 0) == -1){
                perror("recv");
            }
            printf("message - %s\n", buffer);
        }       
    }
    return 0;
}
int main(int argc, char *argv[]){
    int fd, status;
    struct sockaddr_in client;
    struct ip_mreqn mult;
    if(argc < 4){
        printf("Not enough arguments\n");
        return -1;
    }
    if(sscanf(argv[PORT], "%d", &port) != 1){
        printf("Erorr - incorrect source port %s\n", argv[PORT]);
        return -1;
    }
    if((fd = socket(fd, SOCK_DGRAM, 0)) == -1){
        perror("socket");
        return -1;
    }
    printf("Socket - %d", fd);
    memset((char *)&client, 0, sizeof(struct sockaddr_in));
    if(inet_pton(AF_INET, argv[MULTIADDR], &client.sin_addr) <= 0){
        printf("Erorr - incorect source addr %s\n", argv[MULTIADDR]);
        close(fd);
        return -1;
    }
    client.sin_port = htons(port);
    client.sin_family = AF_INET;
    memset((void *)&mult, 0, sizeof(mult));
    mult.imr_ifindex = 0;
    if(inet_pton(AF_INET, argv[MULTIADDR], &mult.imr_multaddr) <= 0){
        perror("multiaddr, inet_pton");
        return -1;
    }
        if(inet_pton(AF_INET, argv[LOCADDR], &mult.imr_address) <= 0){
        perror("locaddr, inet_pton");
        close(fd);
        return -1;
    }
    if(setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mult, sizeof(mult))){
        printf("errno = %d\n", errno);
        perror("setsockopt");
        close(fd);
        return -1;
    }

    if((bind(fd, (struct sockaddr*)&client, sizeof(struct sockaddr_in))) == -1 ){
        perror("bind");
        close(fd);
        return -1;
    }
    char buffer[SIZE_BUF];
    pthread_t id;
    pthread_create(&id, NULL, thread_recvfrom, (void**)&fd);

    while(1){
        printf("Input: ");
        fgets(buffer, SIZE_BUF, stdin);
    }
    pthread_join(id, (void**)&status);
    close(fd);
    printf("End client\n");
    return 0;
}