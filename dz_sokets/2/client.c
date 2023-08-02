#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#define PATH "SERVER"

int main() {
    int fd;
    char buf[100];
    socklen_t client_size;
    struct sockaddr server;
    fd=socket(AF_LOCAL, SOCK_DGRAM, 0);
    if (fd == -1){
        perror ("socket");
        return -1;
    }
    server.sa_family = AF_LOCAL;
    strcpy(server.sa_data, PATH);
    if (connect(fd,(struct sockaddr *)&server, sizeof(server)) == -1){
        perror("connect");
        return -1;
    }
    printf("Input: ");
    fgets(buf, sizeof(buf), stdin);
    if (sendto(fd, buf, sizeof(buf), 0, (struct sockaddr *)&server, sizeof(server)) == -1){
        perror("sendto");
        return -1;
    }
    if (close(fd) == -1){
        perror("close(socket)");
        return -1;
    }
    printf("End\n");
    return 0;
}
