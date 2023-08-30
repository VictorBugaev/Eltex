#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <string.h>

#define MAX_CLIENTS 10
#define MAX_MSG_SIZE 256
#define SERVICE_QUEUE_NAME "/service_queue"

// Структура для хранения информации о клиентах
typedef struct {
    mqd_t queue; // Очередь сообщений для клиента
    char name[MAX_MSG_SIZE]; // Имя клиента
} client_data;

int main() {
    mqd_t service_queue; // Очередь сообщений для сервисных сообщений
    mqd_t client_queues[MAX_CLIENTS]; // Очереди сообщений для клиентов
    client_data clients[MAX_CLIENTS]; // Информация о клиентах
    char buffer[MAX_MSG_SIZE];
    char message[MAX_MSG_SIZE];
    struct mq_attr attr;

    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MSG_SIZE;

    // Создание очереди сервисных сообщений
    service_queue = mq_open(SERVICE_QUEUE_NAME, O_CREAT | O_RDWR, 0644, &attr);
    if (service_queue == -1) {
        perror("mq_open");
        exit(1);
    }

    int num_clients = 0; // Количество подключенных клиентов

    printf("Waiting for clients...\n");

    while (num_clients < MAX_CLIENTS) {
        // Получение сообщения от сервиса
        if (mq_receive(service_queue, buffer, MAX_MSG_SIZE, NULL) == -1) {
            perror("mq_receive");
            exit(1);
        }
      
        // Если получено сообщение о новом клиенте
        if (buffer[0] == 'N') {
            // Открываем очередь сообщений для клиента
            clients[num_clients].queue = mq_open(buffer + 1, O_RDWR);
            if (clients[num_clients].queue == -1) {
                perror("mq_open");
                exit(1);
            }

            // Сохраняем имя клиента
            strcpy(clients[num_clients].name, buffer + 1);

            printf("Client '%s' connected\n", clients[num_clients].name);
            num_clients++; 
            
        } else if (buffer[0] == 'E') { // Если получено сообщение о отключении клиента
            // Находим очередь сообщений нужного клиента и закрываем её
            for (int i = 0; i < num_clients; i++) {
                if (strcmp(clients[i].name, buffer + 1) == 0) {
                    mq_close(clients[i].queue);
                    printf("Client '%s' disconnected\n", clients[i].name);

                    // Сдвигаем остальные клиенты в массиве на его место
                    for (int j = i; j < num_clients - 1; j++) {
                        clients[j] = clients[j + 1];
                    }

                    num_clients--;
                    break;
                }
            }
        } else if (buffer[0] == 'M') { // Если получено сообщение от клиента
            // Ищем отправителя сообщения
            char* sender_name = buffer + 1;
            mqd_t sender_queue;
            for (int i = 0; i < num_clients; i++) {
                if (strcmp(clients[i].name, sender_name) == 0) {
                    sender_queue = clients[i].queue;
                    break;
                }
            }

            // Отправляем сообщение другим клиентам
            for (int i = 0; i < num_clients; i++) {
                if (clients[i].queue != sender_queue) {
                    if (mq_send(clients[i].queue, buffer, MAX_MSG_SIZE, 0) == -1) {
                        perror("mq_send");
                        exit(1);
                    }
                }
            }
        }

        // Отправляем список клиентов на сервисное сообщение
        message[0] = 'L';
        int index = 1;
        for (int i = 0; i < num_clients; i++) {
            strcpy(message + index, clients[i].name);
            index += strlen(clients[i].name) + 1; // +1 для '\0'
        }
        message[index] = '\0';
        if (mq_receive(service_queue, message, MAX_MSG_SIZE, 1) == -1) {
            perror("mq_receive");
            exit(1);
        }
        if (mq_send(service_queue, message, MAX_MSG_SIZE, 0) == -1) {
            perror("mq_send");
            exit(1);
        }
    }

    // Закрытие очереди сервисных сообщений
    mq_close(service_queue);
    mq_unlink(SERVICE_QUEUE_NAME);

    return 0;
}