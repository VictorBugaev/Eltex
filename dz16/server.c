#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#define ADDRESS "192.168.1.15"
#define SERVER_PORT 8888
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    //server_addr.sin_addr.s_addr = htonl(ADDRESS);
    
    inet_pton(AF_INET, ADDRESS, &server_addr.sin_addr);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    
    printf("Server listening on port %d...\n", SERVER_PORT);
    
    while (1) {
       
        ssize_t recv_len = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &client_len);
        if (recv_len == -1) {
            perror("recvfrom");
            exit(EXIT_FAILURE);
        }
        
        buffer[recv_len] = '\0'; 
        
      
        char response[BUFFER_SIZE + 40];
        snprintf(response, BUFFER_SIZE+40, "From server: %s", buffer);
        
   
        ssize_t send_len = sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
        if (send_len == -1) {
            perror("sendto");
            exit(EXIT_FAILURE);
        }
        
        printf("Received: %s\n", buffer);
        printf("Sent: %s\n", response);
    }
    
    close(sockfd);
    
    return 0;
}