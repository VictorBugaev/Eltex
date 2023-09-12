
#include <pthread.h>

#include <netinet/in.h>

#define SERVER "127.0.0.1"
#define PORT 20003
#define SIZEBUF 256
#define COUNT_SERV 10
#define MAX_CLIENTS_IN_SERV 10

#define TIME_POLL_SERV 1000
#define TIME_EPOLL_SERV 1000

#define SIZE_MSG_CHANNEL sizeof(int)

#define SIZE_MSG_CHANNEL_LS sizeof(struct Server_serv *)

struct sockaddr_in;


struct Server_serv{
    struct sockaddr_in server;
    struct Server_serv *next;
    int fd;
    pthread_t id;
    int epollfd;
};

extern int run_server;
extern int fides[2];
extern struct Server_serv list_serv[COUNT_SERV];
extern struct pollfd fd_poll;

struct Server_serv *Create_Server_serv();

void *server_listen(void *argv);
void *server_serving_pull(void *argv);