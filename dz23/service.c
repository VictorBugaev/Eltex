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
    mqd_t service_queue; // Очередь сообщений для сервисных сообщений
    struct mq_attr attr;
    char message[MAX_MSG_SIZE];
    char buffer[MAX_MSG_SIZE];

    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MSG_SIZE;

    // Создание очереди сервисных сообщений
    service_queue = mq_open(SERVICE_QUEUE_NAME, O_CREAT | O_RDWR, 0644, &attr);
    if (service_queue == -1) {
        perror("mq_open");
        exit(1);
    }

    while (1) {
        // Получение сервисного сообщения
        if (mq_receive(service_queue, buffer, MAX_MSG_SIZE, NULL) == -1) {
            perror("mq_receive");
            exit(1);
        }

        // Если получено сообщение о списке клиентов
        if (buffer[0] == 'L') {
            printf("Connected clients: ");
            
            int index = 1;
            while (index < MAX_MSG_SIZE && buffer[index] != '\0') {
                printf("%s ", buffer + index);
                index += strlen(buffer + index) + 1;
            }
            
            printf("\n");
        } else {
            printf("Received message: %s\n", buffer);
        }
        
    }

    // Закрытие очереди сервисных сообщений
    mq_close(service_queue);
    mq_unlink(SERVICE_QUEUE_NAME);

    return 0;
}