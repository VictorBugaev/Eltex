#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8888
#define PACKET_SIZE 100


unsigned short csum(unsigned short *buf, int nwords) {
    unsigned long sum;
    for (sum = 0; nwords > 0; nwords--)
        sum += *buf++;
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return (unsigned short)(~sum);
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char packet[PACKET_SIZE];
    char buffer[PACKET_SIZE];
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    
    struct udphdr *udp_header = (struct udphdr *)(packet + sizeof(struct iphdr));
    udp_header->source = htons(12345); 
    udp_header->dest = htons(SERVER_PORT); 
    udp_header->len = htons(sizeof(struct udphdr)); 
    udp_header->check = 0; 
    

    struct iphdr *ip_header = (struct iphdr *)packet;
    ip_header->version = 4;
    ip_header->ihl = 5;
    ip_header->tos = 0;
    ip_header->tot_len = sizeof(struct iphdr) + sizeof(struct udphdr);
    ip_header->id = htons(54321);
    ip_header->frag_off = 0;
    ip_header->ttl = 255;
    ip_header->protocol = IPPROTO_UDP;
    ip_header->check = 0;
    ip_header->saddr = inet_addr("127.0.0.1"); 
    ip_header->daddr = server_addr.sin_addr.s_addr;
    

    char *data = packet + sizeof(struct iphdr) + sizeof(struct udphdr);
    strcpy(data, "Hello, server!");
    

    udp_header->check = csum((unsigned short *)udp_header, sizeof(struct udphdr));
    

    if (sendto(sockfd, packet, ip_header->tot_len, 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("sendto");
        exit(EXIT_FAILURE);
    }
    
    printf("Packet sent to server.\n");

    struct sockaddr_in response_addr;
    socklen_t response_len = sizeof(response_addr);
    ssize_t recv_len = recvfrom(sockfd, buffer, PACKET_SIZE, 0, (struct sockaddr *)&response_addr, &response_len);
    if (recv_len == -1) {
        perror("recvfrom");
        exit(EXIT_FAILURE);
    }

    if (strcmp(inet_ntoa(response_addr.sin_addr), SERVER_IP) != 0) {
        printf("Received packet from unauthorized server: %s\n", inet_ntoa(response_addr.sin_addr));
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    
    buffer[recv_len] = '\0'; 
    printf("Received from server: %s\n", buffer);
    
    close(sockfd);
    
    return 0;
}