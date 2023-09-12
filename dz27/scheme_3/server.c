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

#define SIZE_MSG_CHANNEL sizeof(int)

int fides[2];
pthread_t id_free_serv;

void sigusr1(int sig, siginfo_t *siginf_pr, void *ptr){
    //BLOCKED
}
void sigusr2(int sig, siginfo_t *siginf_pr, void *ptr){
    //BLOCKED
}
void *server_listen(void *args){
    int fd = *(int*)args;
    struct sockaddr_in client;
    int size = sizeof(client);
    int new_fd;
    char buffer[SIZEBUF];
    memset(buffer, 0, SIZEBUF);
    listen(fd, 5);
    while(1){
        new_fd = accept(fd, (struct sockaddr*)&client, &size);
        if(new_fd < -1){
            perror("accept");
            continue;
        }
        write(fides[1], (char*)&new_fd, SIZE_MSG_CHANNEL);
    }
    return NULL;
}
void *server_serving(void *args){
    int fd = *(int*)args;
    char buffer[SIZEBUF];
    while(1){
        if(recv(fd, buffer, SIZEBUF, 0) == -1 && errno != 0)
            break;
        sprintf(buffer, "msg received");
        if(send(fd, buffer, SIZEBUF, 0) == -1 && errno != 0)
            break;
    }
    close(fd);
    printf("[fd %d]End serving server\n", fd);
    return NULL;
}

void *generate_server_serv(void *args){
    int fd;
    int res;
    pthread_t id;
    while(1){
        res = read(fides[0], (char*)&fd, SIZE_MSG_CHANNEL);
        if(res < SIZE_MSG_CHANNEL){
            printf("Error: incorrect data\n");
            continue;
        }
        if(pthread_create(&id, NULL, server_serving, (void**)&fd) == -1){
            perror("pthread_create");
            continue;
        }
        pthread_detach(id);
    }
    return NULL;
}

int main(int argc, char *argv[]){
    if(argc < 3){
        printf("Not enough arguments\n");
        printf("arguments: address, port\n");
        return -1;
    }
    int port;
    if(sscanf(argv[2], "%d", &port) != 1){
        printf("Incorrect port: %s\n", argv[2]);
        return -1;
    }
    printf("pid = %d\n", getpid());
    printf("Start server3\n");
    struct sockaddr_in server;
    int fd;
    pthread_t id;
    if(inet_pton(AF_INET, argv[1], &server.sin_addr) <= 0){
        printf("Incorrect address: %s\n", argv[1]);
        return -1;
    }
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0){
        perror("socket");
        return -1;
    }
    int setsock = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &setsock, sizeof(setsock))){
        perror("setsockopt");
        return -1;
    }
    if( bind(fd, (struct sockaddr*)&server, sizeof(struct sockaddr_in)) ){
        perror("bind");
        return -1;
    }
    struct sigaction act = { 0 };
    act.sa_sigaction = sigusr1;
    act.sa_flags = SA_SIGINFO;
    if (sigaction(SIGUSR1, &act, NULL) == -1) {
        perror("sigaction");
        return -1;
    }
    struct sigaction act2 = { 0 };
    act2.sa_sigaction = sigusr2;
    act2.sa_flags = SA_SIGINFO;
    if (sigaction(SIGUSR2, &act2, NULL) == -1) {
        perror("sigaction2");
        return -1;
    }
    pipe(fides);
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
    pthread_create(&id, NULL, server_listen, (void**)&fd);
    pthread_t id_gen;
    pthread_create(&id_gen, NULL, generate_server_serv, (void**)NULL);
    
    int sig;
    while(1){
        sigwait(&signalSet, &sig);
        #ifdef DEBUG
            printf("sig = %d\n", sig);
        #endif
        if(sig == SIGTSTP || sig == SIGTERM || sig == SIGINT){
            break;
        }
    }
    close(fd);
    close(fides[0]);  close(fides[1]);
    printf("End server3\n");
}



