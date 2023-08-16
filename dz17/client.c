#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h> 

#define SIZE_UDP 8
#define SIZE_IP 20
#define SIZE_ETH 14
#define SIZE_MES 100
#define SIZE_PACKET (SIZE_UDP + SIZE_IP +SIZE_IP)

enum add{
    DEVICE = 1  ,
    SOURECE_MAC,
    DEST_MAC,
    SOURCE_ADDR,
    DEST_ADDR,
    SOURECE_PORT,
    DEST_PORT
};

int port;
const port_server;
char *addr;
char *addr_server;

void *recv_message(void *args){
    int fd = *(int *)args;
    #ifdef DEBUG
        printf("INIT - %d\n", fd);
    #endif
    char message_server[SIZE_PACKET];
    struct iphdr *iphdr = (struct iphdr *)(message_server + SIZE_ETH);
    struct udphdr *udphdr = (struct udphdr *)(message_server + SIZE_ETH + SIZE_IP);
    int ch_ip, ch_udp;
    char str_destsddr[INET_ADDRSTRLEN], str_sourceaddr[INET_ADDRSTRLEN];
    while (1)
    {
        memset(message_server, 0, sizeof(message_server));
        if(recvfrom(fd, message_server, sizeof(message_server), 0, NULL, NULL) == -1){
            perror("recvfrom");
            return -1;
        }
        inet_ntop(AF_INET, &iphdr -> saddr, str_sourceaddr, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &iphdr -> daddr, str_destsddr, INET_ADDRSTRLEN);
        #ifdef DEBUG{
            printf("[%s:%d] recv :\n", str_sourceaddr, ntohs(udphdr->source));
            print_header(message_server, sizeof(message_server));
            printf("old: ip check sum = %hd\n", iphdr->check);
            printf("udp check sum = %hd\n", udphdr -> check);
        }
        #endif
        ch_ip = iphdr -> check;
        iphdr -> check = 0;
        iphdr -> check = ~check_sum_ip((void *)iphdr, SIZE_IP/2 );
        ch_udp = udphdr -> check;
        check_sum_udp(iphdr, (unsigned short *)(message_server + SIZE_IP + SIZE_ETH));
        #ifdef DEBUG
            printf("new: ip check sum = %hd\n", iphdr -> check);
            printf("udp check sum = %hd\n, " udphdr -> check);
            if(ch_ip != iphdr->check){
                printf("incorrect ip check sum");
            }
            if(ch_udp != udphdr->check){
                printf("incorrect udp check sum");
            }
        #endif
        if((ch_ip == iphdr -> check) && (ch_udp == udphdr -> check) &&\
            (ntohs(udphdr -> source) == port_server ) && (strcmp(str_sourceaddr, addr_server) == 0) &&\
            (ntohs(udphdr -> dest) == port) && (strcmp(str_destsddr, addr) == 0)){
                printf("[%s:%d] message server: %s\n", str_sourceaddr, ntohs(udphdr->source), message_server + SIZE_UDP + SIZE_IP + SIZE_ETH);
            }
    }
    return NULL;
}
void convet_mac(const char *str_mac, char *mac){
        for(int i = 0; i < 18; i +=3){
            sscanf(str_mac + i, "%2hhx", mac + (i/3));
        }
    }
void print_headers(char *pacet, int len){
    for(int i = 0; i<18; i+=3)
        printf("%3d ", (unsigned char)pacet[i]);
    printf("\n\n");
    for(int i = SIZE_ETH; i<SIZE_IP+SIZE_ETH; ++i)
        printf("%3d ", (unsigned char)pacet[i]);
    printf("\n\n");
    for(int i = SIZE_ETH + SIZE_IP; i < SIZE_ETH + SIZE_IP + SIZE_UDP; ++i)
        printf("%3d ", (unsigned char)pacet[i]);
    printf("\n\n");
    printf(" message - %s\n ", pacet + SIZE_ETH + SIZE_IP + SIZE_UDP);
}

short check_sum_ip(void *args, int j){
    int s = 0;
    unsigned short *ptr = (unsigned short *)args;
    for(int i = 0; i < j; ++i){
        s += *ptr;
        ptr ++;
    }
    unsigned short sm = (unsigned short)(s & 0xFFFF) + (unsigned short)(s >> 16);
    sm =(unsigned short)(s & 0xFFFF) + (unsigned short)(s>>16);
    return sm;
}

void check_sum_udp(struct iphdr *iphdr, unsigned short *mes){
    register unsigned long sum = 0;
    struct udphdr *udphdr = (struct udphdr *)(mes);
    unsigned short udpLen = htons(udphdr -> len);
    sum += ((iphdr -> saddr >> 16)& 0xFFFF + ((iphdr -> saddr)& 0xFFFF));
    sum += ((iphdr -> saddr >> 16)& 0xFFFF + ((iphdr -> saddr)& 0xFFFF));
    udphdr -> check = 0;
    while (udpLen>1)
    {
        sum+= *mes++;
        udpLen -= 2;
    }
    if(udpLen > 0){
        sum += ((*mes)&htons(0xFF00));
    }
    while (sum>>16)
    {
        sum = (sum & 0xffff) + (sum >> 16);
    }
    udphdr->check = (unsigned short)~sum;
}
int main(int argc, char *argv[]){
    if(argc < 7){
        printf("Недостаточно аргументов\n");
        printf("\tаргументы - device name, source mac, dest mack, saddr, daddr, source, dest\n");
        printf("End\n");
        return -1;
    }
    unsigned int index_device = if_nametoindex(argv[DEVICE]);
    #ifdef DEBUG
        printf("index = %d\n", index_device);
    #endif
    if(index <= 0){
        printf("Not found device: %s\n", argv[DEVICE]);
        return -1;
    }
    if(sscanf(argv[SOURECE_PORT], "%d", &port) != 1){
        printf("Error: incorrect source port %s\n", argv[SOURECE_PORT]);
        return -1;
    }
    if(sscanf(argv[DEST_PORT], "%d", &port_server) != 1){
        printf("Error: incorrect destination port %s\n", argv[DEST_PORT]);
        return -1;
    }
    int fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if(fd < 0){
        perror("socket");
        return -1;
    }
    char buffer[SIZE_PACKET];
    memset(buffer, 0, sizeof(buffer));
    addr = argv[SOURCE_ADDR];
    addr_server = argv[DEST_ADDR];
    pthread_t id;
    pthread_create(&id, NULL, recv_message, (void**)&fd);

    struct sockaddr_ll addrll;
    memset((void*)&addrll, 0, sizeof(addrll));
    addrll.sll_family = AF_PACKET;
    addrll.sll_protocol = htons(ETH_P_ALL);
    addrll.sll_ifindex = index_device;  
    addrll.sll_halen = ETH_ALEN;
    convert_mac(argv[SOURECE_MAC], (char*)addrll.sll_addr);

    struct ethhdr *ethhdr = (struct ethhdr*)buffer;
    struct iphdr *iphdr = (struct iphdr *)(buffer + SIZE_ETH);
    struct udphdr *udphdr = (struct udphdr*)(buffer + SIZE_IP + SIZE_ETH);

    convert_mac(argv[DEST_MAC], (char*)ethhdr->h_dest);
    convert_mac(argv[SOURECE_MAC], (char*)ethhdr->h_source);
    ethhdr->h_proto = htons(ETH_P_IP);

    iphdr->version = 4;
    iphdr->ihl = 5;
    iphdr->tos = 0;
    iphdr->tot_len = htons(SIZE_MES + SIZE_UDP + SIZE_IP);
    iphdr->id = 45;
    iphdr->frag_off = 0;
    iphdr->ttl = 64;
    iphdr->protocol = 17;
    iphdr->check = 0;
    buffer[6 + SIZE_ETH] = 64;

    udphdr->source = htons(port);
    udphdr->dest = htons(port_server);
    udphdr->len = htons(SIZE_MES + SIZE_UDP);
    udphdr->check = 0;

    if(inet_pton(AF_INET, argv[SOURCE_ADDR], &iphdr->saddr) <= 0){
        printf("Error: incorrect source address %s\n", argv[SOURCE_ADDR]);
        close(fd);
        return -1;
    }
    if(inet_pton(AF_INET, argv[DEST_ADDR], &iphdr->daddr) <= 0){
        printf("Error: incorrect destination address %s\n", argv[DEST_ADDR]);
        close(fd);
        return -1;
    }

    while(1){
    	memset(buffer + SIZE_PACKET - SIZE_MES, 0, SIZE_MES);
        iphdr->check = 0;
        printf("Input: \n");
        fgets(buffer + SIZE_PACKET - SIZE_MES, SIZE_MES, stdin);
        replace_enter(buffer + SIZE_PACKET - SIZE_MES);
        if(!strcmp(buffer + SIZE_PACKET - SIZE_MES, "exit")){
            break;
        }
        iphdr->check = ~check_sum_ip((void*)iphdr, SIZE_IP/2);
        check_sum_udp(iphdr, (unsigned short*)(buffer + SIZE_IP + SIZE_ETH));
        #ifdef DEBUG
            printf("[%s:%d] send:\n", argv[SOURCE_ADDR], port)
            print_headers(buffer, sizeof(buffer));
            printf("ip chek_sum = %hd, ~%hd\n", ~iphdr->check, iphdr->check);
            printf("udp chek_sum = %hd, ~%hd\n", ~udphdr->check, udphdr->check);
        #endif
        if(sendto(fd, buffer, SIZE_PACKET, 0, (struct sockaddr*)&addrll, sizeof(addrll)) == -1){
            perror("sendto");
        }
        printf("sendto [%s|%s|%d]\n", argv[DEST_MAC], argv[DEST_ADDR], port_server);
    }
    if(close(fd)){
        perror("close");
    }
    printf("End client\n");
    return 0;
}
