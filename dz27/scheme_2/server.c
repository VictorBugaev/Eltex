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
#define SIZE_MSG_CHANNEL sizeof(int)

struct Server_serv{
    struct sockaddr_in server;
    int fd;
    pthread_t id;
    int run;
    int index;
};

int count_server;
struct Server_serv **list_serv;

void sigusr1(int sig, siginfo_t *siginf_pr, void *ptr){
    //BLOCKED
}
void sigusr2(int sig, siginfo_t *siginf_pr, void *ptr){
    //BLOCKED
}
void *server_listen(void*);
void *server_serving_dinamic(void*);
void *server_serving_static(void*);

int main(int argc, char *argv[]){
    if(argc < 4){
        printf("Not enough arguments\n");
        printf("arguments: address, port, count servers\n");
        return -1;
    }
    int port;
    if(sscanf(argv[2], "%d", &port) != 1){
        printf("Incorrect port: %s\n", argv[2]);
        return -1;
    }
    if(sscanf(argv[3], "%d", &count_server) != 1){
        printf("Incorrect port: %s\n", argv[3]);
        return -1;
    }
    printf("pid = %d\n", getpid());
    printf("Start server2\n");
    int fd;
    pthread_t id;
    struct sockaddr_in server;
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
    printf("Create server listen\n");
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
    printf("Create sigusr1, sigusr2\n");
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
    printf("Create list ignore signal\n");
    list_serv = (struct Server_serv**)malloc(sizeof(struct Server_serv*) * count_server);
    for(int i = 0; i < count_server; ++i){
        list_serv[i] = (struct Server_serv*)malloc(sizeof(struct Server_serv));
        list_serv[i]->run = 0;
        list_serv[i]->index = i;
        pthread_create(&list_serv[i]->id, NULL, server_serving_static, (void**)list_serv[i]);
    }
    printf("Create pull thread\n");
    
    pthread_create(&id, NULL, server_listen, (void**)&fd);
    int sig;
    while(1){
        sigwait(&signalSet, &sig);
        printf("sig = %d\n", sig);
        if(sig == SIGTSTP || sig == SIGTERM || sig == SIGINT)
            break;
    }
    printf("Clear resource\n");
    close(fd);
    for(int i = 0; i < count_server; ++i){
        pthread_kill(list_serv[i]->id, SIGUSR2);
        close(list_serv[i]->fd);
    }
    sleep(1);
    printf("End server1\n");
}

void *server_listen(void *args){
    int fd = *(int*)args;
    listen(fd, 5);
    struct sockaddr_in client;
    int size = sizeof(client);
    int new_fd;
    char buffer[SIZEBUF];
    memset(buffer, 0, SIZEBUF);
    int i;
    pthread_t id;
    while(1){
        new_fd = accept(fd, (struct sockaddr*)&client, &size);
        if(new_fd < -1){
            perror("accept");
            continue;
        }
        for(i = 0; i < count_server; ++i){
            if(!list_serv[i]->run)
                break;
        }
        if(i < count_server){
            list_serv[i]->run = 1;
            list_serv[i]->fd = new_fd;
            pthread_kill(list_serv[i]->id, SIGUSR1);
            continue;
        }
        if(pthread_create(&id, NULL, server_serving_dinamic, (void**)&new_fd) == -1){
            perror("pthread_create");
            close(new_fd);
            continue;
        }
        pthread_detach(id);
    }
    return NULL;
}
void *server_serving_dinamic(void *args){
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
    printf("[fd %d] End serving server\n", fd);
    return NULL;
}
void *server_serving_static(void *args){
    struct Server_serv *serv = (struct Server_serv*)args;
    char buffer[SIZEBUF];
    sigset_t signalSet;
    if(sigemptyset(&signalSet)){
        perror("ERROR sigemptyset");
        return NULL;
    }
    sigaddset(&signalSet, SIGUSR1);
    sigaddset(&signalSet, SIGUSR2);
    sigaddset(&signalSet, SIGPIPE);
    pthread_sigmask(SIG_SETMASK, &signalSet, NULL);
    int sig;
    while(1){
        #ifdef DEBUG
            printf("[%d] wait\n",serv->index);
        #endif 
        sigwait(&signalSet, &sig);
        if(sig == SIGPIPE){
            printf("[%d] disconnect client\n", serv->index);
            continue;
        }
        if(sig == SIGUSR2)
            break;
        while(1){
            if(recv(serv->fd, buffer, SIZEBUF, 0) == -1 && errno != 0){
                perror("recv");
                break;
            }
            sprintf(buffer, "msg received");
            if(send(serv->fd, buffer, SIZEBUF, 0) == -1 && errno != 0){
                break;
            }
        }
        close(serv->fd);
        printf("[%d] close fd\n", serv->index);
        serv->run = 0;
    }
    printf("End serving server: %d\n", (int)serv->index);
    return NULL;
}