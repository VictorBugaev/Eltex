#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <pthread.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SIZEBUF 200

char addr[] = "192.168.28.154";
int port = 4567;

#define DEBUG

void replace_enter(char *str){
    int i = 0;
    while('\0' != str[i]){
        if(str[i] == '\n'){
            str[i] = '\0';
            break;
        }
        ++i;
    }
}

void *thread_server(void *args){
    int fd = *(int*)args;
    char buffer[SIZEBUF + 10];
    char msg_client[SIZEBUF];
    char str[INET_ADDRSTRLEN];
    int count_package = 0;
    struct sockaddr_in client;
    memset((char*)&client, 0, sizeof(struct sockaddr_in));
    int size = sizeof(struct sockaddr_in);

    printf("Start server: fd:%d, %s:%d\n", fd, addr, port);
    
    while(1){
        memset(msg_client, 0, SIZEBUF);
        if(recvfrom(fd, msg_client, SIZEBUF, 0, (struct sockaddr*)&client, (socklen_t*)&size) == -1){
            perror("recvform");
        }
        inet_ntop(AF_INET, &client.sin_addr, str, INET_ADDRSTRLEN);
        
        if(ntohs(client.sin_port) != port && (strcmp(str, addr) != 0)){
            count_package++;
            printf("count packege = %d\n", count_package);
            printf("[%s:%d] ", str, ntohs(client.sin_port));
            printf("client: %s\n", msg_client);
            memset(buffer, 0, sizeof(buffer));
            sprintf(buffer,"server-%s", msg_client);
            
            if(sendto(fd, buffer, SIZEBUF, 0, (struct sockaddr*)&client, (socklen_t)size) == -1){
                perror("sendto");
            }
        }
        else{
            #ifdef DEBUG
                printf("[%s:%d] ", str, ntohs(client.sin_port));
                printf("msg: %s\n", msg_client);
            #endif
        }
    }
    return NULL;
}
int main(){
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
    char buffer_com[10];
    while(1){
        printf("Input: ");
        fgets(buffer_com, sizeof(buffer_com), stdin);
        replace_enter(buffer_com);
        if(!strcmp(buffer_com, "exit")){
            break;
        }
    }
    if(close(fd)){
        perror("close");
    }
    printf("End server\n");
    return 0;
}