#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>

#define SERVER_IP "192.168.1.15"
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
    char packet[PACKET_SIZE];
    struct iphdr *ip_header = (struct iphdr *)packet;
    struct udphdr *udp_header = (struct udphdr *)(packet + sizeof(struct iphdr));
    struct sockaddr_in server_addr;
    char buffer[PACKET_SIZE];
    
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (sockfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    
    int setsock = 1;
    if(setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &setsock, sizeof(setsock))){
        perror("setsockopt");
        return -1;
    }
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    
   
    udp_header->source = htons(8888);
    udp_header->dest = htons(SERVER_PORT);
    udp_header->len = htons(sizeof(struct udphdr)); 
    udp_header->check = 0; 
    
   
    ip_header->ihl = 5;
    ip_header->version = 4;
    ip_header->tos = 0;
    ip_header->tot_len = PACKET_SIZE;//sizeof(struct iphdr) + sizeof(struct udphdr);
    ip_header->id = 45;
    ip_header->frag_off = 0;
    ip_header->ttl = 64;
    packet[6] = 64;
    ip_header->protocol = IPPROTO_UDP;
    ip_header->check = 0;
    inet_pton(AF_INET, "192.168.1.14", &ip_header->saddr);
    //ip_header->saddr = inet_addr("192.168.1.14"); // IP отправителя
    //ip_header->daddr = server_addr.sin_addr.s_addr;
    inet_pton(AF_INET, SERVER_IP, &ip_header->daddr);
    
    // Заполнение контрольной суммы IP заголовка
    ip_header->check = 0;//csum((unsigned short *)ip_header, sizeof(struct iphdr) / 2);
    
    // Заполнение данных
    char *data = packet + sizeof(struct iphdr) + sizeof(struct udphdr);
    strcpy(data, "Hello, server!");
    
    // Вычисление контрольной суммы UDP пакета и установка её
    udp_header->check = 0;//csum((unsigned short *)udp_header, sizeof(struct udphdr));
    
    // Отправка пакета
    if (sendto(sockfd, packet, ip_header->tot_len, 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("sendto");
        exit(EXIT_FAILURE);
    }
    
    printf("Packet sent to server.\n");
    
    // Принятие ответа от сервера, только если IP-адрес соответствует ожидаемому
    //ssize_t recv_len = recv(sockfd, buffer, PACKET_SIZE, 0);
    memset(buffer, 0, PACKET_SIZE);
    ssize_t recv_len = recvfrom(sockfd, buffer, PACKET_SIZE, 0, NULL, NULL);
    
    if (recv_len == -1) {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    
    printf("Received from server: %s\n", buffer+28);
    
    close(sockfd);
    
    return 0;
}