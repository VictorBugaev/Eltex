#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SIZE_BUF 250

char *addr;
int port;

void *thread_server(void *args){
    int fd = *(int*)args;
    int size = sizeof(struct sockaddr_in);
    int count = 0;
    char buffer[SIZE_BUF + 10];
    char mes[SIZE_BUF];
    char str[INET_ADDRSTRLEN];
    struct sockaddr_in client;
    memset((char*)&client, 0, size);
    printf("Start server: fd -%d", fd);

    while(1){
        memset(mes, 0, SIZE_BUF);
        if(recvfrom(fd, mes, SIZE_BUF, 0, (struct sockaddr*)&client, (socklen_t*)&size) == -1){
            perror ("recvfrom");
            return -1;
        }
        inet_ntop(AF_INET, &client.sin_addr, str, INET_ADDRSTRLEN);
        if (ntohs(client.sin_port) != port && (strcmp(str, addr) != 0))
        {
            count ++;
            printf("count package - %d\n", count);
            printf("[%s:%d] ", str, ntons(client.sin_port));
            printf("client - %s\n", mes);
            memset(buffer, 0, sizeof(buffer));
            sprintf(buffer, "server - %s", mes);
        }
        if(sendto(fd, buffer, SIZE_BUF, 0, (struct sockaddr*)&client, (socklen_t)size) == -1){
            perror ("sendto");
        }
        else{
            #ifdef DEBUG
                printf("[%s:%d]", str, ntohs(client.sin_port));
                printf("message - %s\n", mes);
            #endif
        }
    }
    return NULL;
}
int main(int argc, char *argv[]){
    if(argc < 3){
        printf("Not enough arguments\n");
        printf("\targuments: addr, port\n");
        printf("Exit\n");
        return -1;
    }
    addr = argv[1];
    if(sscanf(argv[2], "%d", &port) <= 0){
        printf("Error: incorrect port %s\n", argv[2]);
        return -1;
    }

    struct sockaddr_in server;
    memset((char*)&server, 0, sizeof(struct sockaddr_in));
    if(inet_pton(AF_INET, addr, &server.sin_addr) <= 0){
        printf("Error: incorrect address %s\n", addr);
        return -1;
    }
    server.sin_port = htons(port);
    server.sin_family = AF_INET;

    int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(fd < 0){
        perror("socket");
        return -1;
    }
    if(bind(fd, (struct sockaddr*)&server, sizeof(struct sockaddr_in)) ){
        perror("bind");
        return -1;
    }
	
    pthread_t id_server;

    pthread_create(&id_server, NULL, thread_server, (void**)&fd);
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
    int sig;
    while(1){
        sigwait(&signalSet, &sig);
        printf("sig = %d\n", sig);
        if(sig == SIGTSTP || sig == SIGTERM || sig == SIGINT){
            break;
        }
    }
    if(close(fd)){
        perror("close");
    }
    printf("End server\n");
    return 0;
}