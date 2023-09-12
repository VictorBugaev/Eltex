#include "server.h"

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

int fides[2];
struct pollfd fd_poll;
struct Server_serv list_serv[COUNT_SERV];
int run_server = 1;

struct Server_serv *Create_Server_serv(){
    struct Server_serv *new_elem = (struct Server_serv*)malloc(sizeof(struct Server_serv));
    new_elem->next = NULL;
    return new_elem;
}

void *server_listen(void *argv){
    struct Server_serv *serv = (struct Server_serv*)argv;
    listen(serv->fd, 5);
    struct sockaddr_in client;
    int size = sizeof(client);
    int new_fd;
    while(1){
        new_fd = accept(serv->fd, (struct sockaddr*)&client, &size);
        if(new_fd < -1){
            perror("accept");
            continue;
        }
        write(fides[1], (char*)&new_fd, SIZE_MSG_CHANNEL);
    }
    return NULL;
}
void *server_serving_pull(void *argv){
    struct Server_serv *serv = (struct Server_serv*)argv;
    char buffer[SIZEBUF];
    sigset_t signalSet;
    if(sigemptyset(&signalSet)){
        perror("ERROR sigemptyset");
        return NULL;
    }
    sigaddset(&signalSet, SIGPIPE);
    pthread_sigmask(SIG_SETMASK, &signalSet, NULL);
    
    int run = 1;
    int sig;
    int count_client = 0;
    int fd;
    struct epoll_event pevents[MAX_CLIENTS_IN_SERV];
    struct epoll_event events = { 0 };
    events.events = EPOLLIN;
    int res;
    while(1){
        if(!run_server){
            break;
        }
        if(count_client < MAX_CLIENTS_IN_SERV){
            int res = poll(&fd_poll, 1, TIME_POLL_SERV);
            if(res == -1){
                sprintf(buffer, "[%d] poll", serv->id);
                perror(buffer);
            }
            else if(res == 0){
                #ifdef DEBUG
                    printf("[%d] fd not found\n", serv->id);
                #endif
            }
            else{
                res = read(fides[0], (char*)&fd, SIZE_MSG_CHANNEL);
                if(res == SIZE_MSG_CHANNEL){
                    events.data.fd = fd;
                    if(epoll_ctl(serv->epollfd, EPOLL_CTL_ADD, fd, &events)){
                        sprintf(buffer, "[%d] epoll_ctl add fd(%d)", serv->id, fd);
                        perror(buffer);
                        continue;
                    }
                    count_client++;
                }
                else{
                    #ifdef DEBUG
                        printf("[%d] data incorrect\n", serv->id);
                    #endif
                }
            }
        }
        if(count_client > 0){
            res = epoll_wait(serv->epollfd, pevents, MAX_CLIENTS_IN_SERV, TIME_EPOLL_SERV);
            if(res == -1){
                sprintf(buffer, "[%d] epoll_wait", serv->id);
                perror(buffer);
            }
            else if(res == 0){
                #ifdef DEBUG
                    printf("[%d] events not found\n", serv->id);
                #endif
            }
            else{
                #ifdef DEBUG
                    printf("[%d] epwres = %d\n", serv->id, res);
                #endif
                for(int i = 0; i < res; ++i){
                    #ifdef DEBUG
                        printf("[%d] [%d] ptr = %d\n", serv->id, i, pevents[i].data.fd);
                    #endif
                    if(pevents[i].events & EPOLLIN ){
                        fd = pevents[i].data.fd;
                        if(recv(fd, buffer, SIZEBUF, 0) == -1 && errno != 0){
                            #ifdef DEBUG
                                printf("[%d] disconnect client fd: %d\n", serv->id, fd);
                            #endif
                            if(epoll_ctl(serv->epollfd, EPOLL_CTL_DEL, fd, &events)){
                                sprintf(buffer, "[%d] epoll_ctl del fd(%d)", serv->id, fd);
                                perror(buffer);
                            }
                            close(fd);
                            count_client--;
                            continue;
                        }
                        #ifdef DEBUG
                            printf("[%d] fd: %d, msg: %s\n", serv->id, fd, buffer);
                        #endif
                        sprintf(buffer, "msg received");
                        if(send(fd, buffer, SIZEBUF, 0) == -1 && errno != 0){
                            #ifdef DEBUG
                                printf("[%d] Error send - fd: %d, disconnect client\n", serv->id, fd);
                            #endif
                            if(epoll_ctl(serv->epollfd, EPOLL_CTL_DEL, fd, &events)){
                                sprintf(buffer, "[%d] epoll_ctl del fd(%d)", serv->id, fd);
                                perror(buffer);
                            }
                            close(fd);
                            count_client--;
                        }
                    }
                    else{
                        #ifdef DEBUG
                            printf("[%d] [%d] epwptr = %d, XZ chto tyt\n", serv->id, i, pevents[i].data.fd);
                        #endif
                    }
                }
            }
        }
    }
    printf("End serving server: %d\n", (int)serv->id);
    return NULL;
}





