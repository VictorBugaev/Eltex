#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

#include <unistd.h> 
#include <string.h>
#include <errno.h>

#include <pthread.h>
#include <signal.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define SIZEBUF 256
#define DEBUG

void *server_serving(void*);

void *server_listen(void *args){
    int fd = *(int*)args;
    struct sockaddr_in client;
    int size = sizeof(client);
    int new_fd;
    pthread_t id;
    char buffer[SIZEBUF];
    memset(buffer, 0, SIZEBUF);
    listen(fd, 5);
    printf("INIT server listen\n");
    while(1){
        new_fd = accept(fd, (struct sockaddr*)&client, &size);
        if(new_fd < -1){
            perror("accept");
            continue;
        }
        if(pthread_create(&id, NULL, server_serving, (void**)&new_fd) == -1){
            perror("pthread_create");
            close(new_fd);
            continue;
        }
        pthread_detach(id);
    }
    return NULL;
}

void *server_serving(void *args){
    int fd = *(int*)args;
    char buffer[SIZEBUF];
    while(1){
        if(recv(fd, buffer, SIZEBUF, 0) == -1 && errno != 0){
            break;
        }
        sprintf(buffer, "msg received");
        if(send(fd, buffer, SIZEBUF, 0) == -1 && errno != 0){
            break;
        }
    }
    close(fd);
    printf("[fd %d]End serving server\n", fd);
    return NULL;
}
int main(int argc, char *argv[]){
    if(argc < 3){
        printf("Not enough arguments\n");
        printf("arguments: address, port\n");
        return -1;
    }
    int port;
    struct sockaddr_in server;
    int fd;
    pthread_t id;
    int setsock = 1;
    int *status;
    int sig;
    if(sscanf(argv[2], "%d", &port) != 1){
        printf("Incorrect port: %s\n", argv[2]);
        return -1;
    }
    printf("pid = %d\n", getpid());
    printf("Start server1\n");

    if(inet_pton(AF_INET, argv[1], &(server.sin_addr)) <= 0){
        printf("Incorrect address: %s\n", argv[1] );
        return -1;
    }
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0){
        perror("socket");
        return -1;
    }
    if(bind(fd, (struct sockaddr*)&server, sizeof(struct sockaddr_in)) ){
        perror("bind");
        return -1;
    }
    if(setsockopt(
        fd, SOL_SOCKET, SO_REUSEADDR, &setsock, sizeof(setsock))) 
        {
        perror("setsockopt");
        return -1;
    }
    pthread_create(&id, NULL, server_listen, (void**)&fd);
    sigset_t signalSet;
    if(sigemptyset(&signalSet)){
        perror("sigemptyset");
        return -1;
    }
    if(sigfillset(&signalSet)){
        perror("sigfillset");
        return -1;
    }
    sigprocmask(SIG_SETMASK, &signalSet, NULL);
    while(1){
        sigwait(&signalSet, &sig);
        printf("sig = %d\n", sig);
        if(sig == SIGTSTP || sig == SIGTERM || sig == SIGINT){
            break;
        }
    }
    pthread_kill(id, SIGINT);
    sleep(1);
    close(fd);
    printf("End server1\n");
}



