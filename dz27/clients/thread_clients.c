#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <time.h>

#include <unistd.h> 
#include <string.h>
#include <errno.h>

#include <pthread.h>
#include <signal.h>
#include <sys/poll.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SIZEBUF 256

struct param_client{
    int sec_start;
    int count_msg;
    int interval;
    pthread_t id;
};
char *address;
int port;
int mass = 0;
int active_client = 0;
int connect_client = 0;
pthread_mutex_t mutex_set_count_client = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_set_connect = PTHREAD_MUTEX_INITIALIZER;

double delta_time(struct timespec *start, struct timespec *end){
    double time_spent = (end->tv_sec - start->tv_sec) +
                        (end->tv_nsec - start->tv_nsec) / (double)1e9;
    return time_spent;
}
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
void *thread_client(void *args){
    struct param_client *prm = (struct param_client*)args;
    sigset_t signalSet;
    if(sigemptyset(&signalSet)){
        perror("ERROR sigemptyset");
        return NULL;
    }
    sigaddset(&signalSet, SIGPIPE);
    pthread_sigmask(SIG_SETMASK, &signalSet, NULL);
    int fds = socket(AF_INET, SOCK_STREAM, 0);
    if(fds < 0){
        perror("socket");
        return NULL;
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, address, &(addr.sin_addr));
    char buf[SIZEBUF];
    while(mass){}
    sleep(prm->sec_start);
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
    if(connect(fds, (struct sockaddr*)&addr, sizeof(addr))){
        perror("connect listen server");
        return NULL;
    }
    pthread_mutex_lock(&mutex_set_connect);
    connect_client++;
    printf("Connect client: %d\n", connect_client);
    pthread_mutex_unlock(&mutex_set_connect);
    clock_gettime(CLOCK_REALTIME, &end);
    double t_con = delta_time(&start, &end);
    printf("id:%d, Connect, time = %f\n",prm->id, t_con);
    int i = prm->count_msg;
    struct pollfd fd_poll;
    fd_poll.fd = fds;
    fd_poll.events = POLLIN;
    int res;
    clock_gettime(CLOCK_REALTIME, &start);
    while(i){
        sprintf(buf, "msg %d", i);
        res = poll(&fd_poll, 1, 1000);
        if(res == -1){
            sprintf(buf, "fd:%d, id: %d, poll", fds, prm->id);
            perror(buf);
        }
        else if(res == 0){  }
        else{
            if(recv(fds, buf, SIZEBUF, 0) == -1){
                sprintf(buf, "fd:%d, id: %d, recv", fds, prm->id);
                perror(buf);
                break;
            }
        }
        if(send(fds, buf, SIZEBUF, 0) == -1){
            sprintf(buf, "fd:%d, id: %d, send", fds, prm->id);
            perror(buf);
            break;
        }
        sleep(prm->interval);
        --i;
    }
    clock_gettime(CLOCK_REALTIME, &end);
    printf("id:%d, Connect: %f, serviсe = %f\n",prm->id, t_con, delta_time(&start, &end));
    printf("End client %d\n", (int)prm->id);
    close(fds);
    pthread_mutex_lock(&mutex_set_count_client);
    active_client--;
    #ifdef DEBUG
        printf("active_client = %d\n", active_client);
    #endif
    if(active_client < 1){
        #ifdef DEBUG
            printf("send signal main thread\n");
        #endif
        kill(getpid(), SIGTERM);
    }
    pthread_mutex_unlock(&mutex_set_count_client);
    return NULL;
}
int main(int argc, char *argv[]){
    srand(time(NULL));
    printf("pid = %d\n", getpid());
    //address | port | count client | mass | condition random_prm | count msg | time_start | interval
    for(int i = 0; i < argc; ++i){
        printf("%s ", argv[i]);
    }
    printf("\n");
    if(argc < 5){
        printf("no arguments specified\n");
        return -1;
    }
    struct in_addr addr;
    address = argv[1];
    if(inet_pton(AF_INET, address, &addr) <= 0){
        printf("Incorrect addres server: %s\n", address);
        return -1;
    }
    if(sscanf(argv[2], "%d", &port) != 1){
        printf("Incorrect port: %s\n", argv[2]);
        return -1;
    }
    int index = 3;
    int count_client = 100, con_random_prm = 1;
    if(sscanf(argv[index++], "%d", &count_client) != 1){
        printf("Incorrect count client\n");
        return -1;
    }
    if(sscanf(argv[index++], "%d", &mass) != 1){
        printf("Incorrect condition mass, (value 0 or 1), default: 0\n");
        mass = 0;
    }
    int count_msg = 10, tsec_start = 1, interval = 1;
    if(argc >= 9){
        sscanf(argv[index++], "%d", &con_random_prm);
        sscanf(argv[index++], "%d", &count_msg);
        sscanf(argv[index++], "%d", &tsec_start);
        sscanf(argv[index++], "%d", &interval);
    }    
    struct param_client *clients = (struct param_client*)malloc(sizeof(struct param_client) * count_client);
    if(con_random_prm){
        for(int i = 0; i < count_client; ++i){
            clients[i].sec_start = (rand() % (tsec_start)) + 1;
            clients[i].count_msg = (rand() % (count_msg)) + 1;
            clients[i].interval = (rand() % (interval)) + 1;
        }
    }
    else{
        for(int i = 0; i < count_client; ++i){
            clients[i].sec_start = tsec_start;
            clients[i].count_msg = count_msg;
            clients[i].interval = interval;
        }
    }
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
    for(int i = 0; i < count_client; ++i){
        if(pthread_create(&clients[i].id, NULL, thread_client, (void**)&clients[i]) == -1){
            perror("pthread_create");
        }
        else
            active_client++;
    }
    mass = 0;
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
        #ifdef DEBUG
            printf("sig = %d\n", sig);
        #endif
        if(sig == SIGTSTP || sig == SIGTERM || sig == SIGINT){
            break;
        }
    }
    pthread_mutex_lock(&mutex_set_count_client);
    int *status;
    if(active_client == 0){
        pthread_mutex_unlock(&mutex_set_count_client);
        for(int i = 0; i < count_client; ++i){
            pthread_join(clients[i].id, (void*)&status);
        }
    }
    clock_gettime(CLOCK_REALTIME, &end);
    printf("count clients = %d, time serving = %f\n", count_client, delta_time(&start, &end));
    printf("End client\n");
    return 0;
}







