#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

#include <unistd.h> 
#include <string.h>
#include <errno.h>

#include <pthread.h>
#include <signal.h>

#include <sys/epoll.h>
#include <sys/poll.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "server.h"

//BLOCKED
void sigusr1(int sig, siginfo_t *siginf_pr, void *ptr){}
//BLOCKED
void sigusr2(int sig, siginfo_t *siginf_pr, void *ptr){}

int main(){
    run_server = 1;
    printf("pid = %d\n", getpid());
    printf("Start server4\n");
    pipe(fides);
    fd_poll.fd = fides[0];
    fd_poll.events = POLLIN;
    struct Server_serv head;
    inet_pton(AF_INET, SERVER, &(head.server.sin_addr));
    head.server.sin_family = AF_INET;
    head.server.sin_port = htons(PORT);
    head.fd = socket(AF_INET, SOCK_STREAM, 0);
    if(head.fd < 0){
        perror("socket");
        return -1;
    }
    if( bind(head.fd, (struct sockaddr*)&head.server, sizeof(struct sockaddr_in)) ){
        perror("bind");
        return -1;
    }
    
    #ifdef DEBUG
        printf("bind server listen\n");
    #endif
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
    #ifdef DEBUG
        printf("Create sigusr(1,2)\n");
    #endif
    for(int i = 0; i < COUNT_SERV; ++i){
        list_serv[i].epollfd = epoll_create(0x77);
        pthread_create(&list_serv[i].id, NULL, server_serving_pull, (void**)&list_serv[i]);
    }
    #ifdef DEBUG
        printf("Start pull thread serving server\n");
    #endif
    sigset_t signalSet;
    if(sigemptyset(&signalSet)){
        perror("sigemptyset");
        return -1;
    }
    if(sigfillset(&signalSet)){
        perror("sigfillset");
        return -1;
    }
    //sigprocmask(SIG_SETMASK, &signalSet, NULL);
    pthread_sigmask(SIG_SETMASK, &signalSet, NULL);
    #ifdef DEBUG
        printf("Create list ignore signal\n");
    #endif
    int setsock = 1;
    if (setsockopt(head.fd, SOL_SOCKET, SO_REUSEADDR, &setsock, sizeof(setsock))){
        perror("setsockopt");
        return -1;
    }

    pthread_create(&head.id, NULL, server_listen, (void**)&head);
    #ifdef DEBUG
        printf("Create: server listen\n");    
    #endif
    
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
    #ifdef DEBUG
        printf("Clear resourse\n");
    #endif
    pthread_kill(head.id, SIGINT);
    run_server = 0;
    sleep(5);
    close(head.fd);
    for(int i = 0; i < COUNT_SERV; ++i){
        close(list_serv[i].epollfd);
    }
    close(fides[0]);  close(fides[1]);
    printf("End server4\n");
    return 0;
}
