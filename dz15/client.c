#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 45678
#define BUFFER_SIZE 1024

struct udpheader {
    uint16_t src_port;
    uint16_t dest_port;
    uint16_t length;
    uint16_t checksum;     
};

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    char message[BUFFER_SIZE + sizeof(struct udpheader)];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    while (1) {
        printf("Enter a message: ");
        fgets(buffer, BUFFER_SIZE, stdin);

        struct udpheader udp_hdr;
        udp_hdr.src_port = htons(11111); 
        udp_hdr.dest_port = htons(SERVER_PORT); 
        udp_hdr.length = htons(sizeof(struct udpheader) + strlen(buffer));
        udp_hdr.checksum = 0;

        memcpy(message, &udp_hdr, sizeof(struct udpheader));
        memcpy(message + sizeof(struct udpheader), buffer, strlen(buffer) + 1);

        sendto(sockfd, message, sizeof(struct udpheader) + strlen(buffer) + 1, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));

        memset(buffer, 0, sizeof(buffer));
        recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&server_addr, NULL);

        printf("Server response: %s\n", buffer );
    }
    
    close(sockfd);
    return 0;
}
