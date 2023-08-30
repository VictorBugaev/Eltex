
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#define MAX_MSG_SIZE 256
#define SERVICE_QUEUE_NAME "/service_queue"

int main() {
    pid_t pid;
    unsigned int priority;
    mqd_t service_queue; // Очередь сообщений для сервисных сообщений
    mqd_t client_queue; // Очередь сообщений для клиента
    char buffer[MAX_MSG_SIZE];
    char message[MAX_MSG_SIZE];
    struct mq_attr attr;
    char client_name[MAX_MSG_SIZE];
    char receive_mes[MAX_MSG_SIZE];
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MSG_SIZE;
    
    pid = getpid();

    printf("My PID - %d\n", pid);
    // Генерируем имя для клиента
    sprintf(client_name, "/client_%d", pid);

    // Создание очереди сообщений для клиента
    client_queue = mq_open(client_name, O_CREAT | O_RDWR, 0644, &attr);
    if (client_queue == -1) {
        perror("mq_open");
        exit(1);
    }

    // Отправляем сервисное сообщение о новом клиенте
    strcpy(message, "N");
    strcat(message, client_name);

    service_queue = mq_open(SERVICE_QUEUE_NAME, O_RDWR);
    if (service_queue == -1) {
        perror("mq_open");
        exit(1);
    }

    if (mq_send(service_queue, message, MAX_MSG_SIZE, 0) == -1) {
        perror("mq_send");
        exit(1);
    }

    printf("Connected to server\n");

    
    while (1) {
        printf("Waiting for messages...\n");
        printf("Enter the message: ");
        fgets(message, MAX_MSG_SIZE, stdin);
        if(mq_send(client_queue, message, MAX_MSG_SIZE, 0) == -1){
            perror("mq_open");
            exit(1);
        }
        // Получаем сообщения от сервера
        if (mq_receive(client_queue,receive_mes, MAX_MSG_SIZE, &priority) == -1) {
            perror("mq_receive");
            exit(1);
        }

        // Если получено сообщение о списке клиентов
        if (buffer[0] == 'L') {
            printf("Connected clients: %s\n", buffer + 1);
        } else {
            printf("Received message: %s\n", buffer);
        }
    

    // Отправляем сервисное сообщение об отключении клиента
    strcpy(message, "E");
    strcat(message, client_name);

    if (mq_send(service_queue, message, MAX_MSG_SIZE, 0) == -1) {
        perror("mq_send");
        exit(1);
    }
}
    // Закрытие очереди сообщений для клиента
    mq_close(client_queue);
    mq_unlink(client_name);

    // Закрытие очереди сервисных сообщений
    mq_close(service_queue);

    return 0;
}