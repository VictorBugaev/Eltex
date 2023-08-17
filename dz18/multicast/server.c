#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SIZEBUF 200

enum addr{
    LOCADDR = 1,
    MULTIADDR,
    PORT,

};
int main(int argc, char *argv[]){
    int d_port, fd;
    char buffer[SIZEBUF];
    struct sockaddr_in client;
    struct in_addr localaddr;
    memset((void*)&client, 0, sizeof(client));
    if(argc < 4){
        printf("Not enough arguments: locaddr, multi address, dest port\n");
        return -1;
    }
    if(sscanf(argv[PORT], "%d", &d_port) != 1){
        printf("Error: incorrect dest port %s\n", argv[PORT]);
        return -1;
    }
    if(d_port < 1025 || d_port > 0xFFFF){
        printf("Error port");
        return -1;
    }
     ;
    if((fd = socket(AF_INET, SOCK_DGRAM, 0))  == -1){
        perror("socket");
        return -1;
    }
    client.sin_port = htons(d_port);
    client.sin_family = AF_INET;
    if(inet_pton(AF_INET, argv[MULTIADDR], &client.sin_addr) <= 0){
        printf("Error: incorrect dest address %s\n", argv[MULTIADDR]);
        close(fd);
        return -1;
    }
    
    if(inet_pton(AF_INET, argv[LOCADDR], &localaddr) <= 0){
        printf("Error: incorrect local address %s\n", argv[LOCADDR]);
        close(fd);
        return -1;
    }
    if((setsockopt(fd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&localaddr, sizeof(localaddr))) == -1){
        perror("setsockopt");
        close(fd);
        return -1;
    }
    while(1){
        printf("Input: ");
        memset(buffer, 0, SIZEBUF);
        fgets(buffer, SIZEBUF, stdin);
        if(sendto(fd, buffer, SIZEBUF, 0, (struct sockaddr*)&client, sizeof(client)) == -1){
            perror("sendto");
            return -1;
        }
    }
    close(fd);
    printf("End server\n");
    return 0;
}