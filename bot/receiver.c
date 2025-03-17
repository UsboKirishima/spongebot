#define _GNU_SOURCE

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <receiver.h>

#define SERVER_PORT 8080

int receiver_init(int *client_fd, struct sockaddr_in *server_address,
                  const char *server_host)
{
    struct addrinfo hints, *res;
    int status;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP

    status = getaddrinfo(server_host, NULL, &hints, &res);
    if (status != 0)
    {
#ifdef DEBUG
        fprintf(stderr, "[receiver] getaddrinfo error: %s\n", gai_strerror(status));
#endif
        return -1;
    }

    memset(server_address, 0, sizeof(*server_address));
    server_address->sin_family = AF_INET;
    server_address->sin_port = htons(SERVER_PORT);
    memcpy(&server_address->sin_addr,
           &((struct sockaddr_in *)res->ai_addr)->sin_addr,
           sizeof(struct in_addr));

    freeaddrinfo(res);

    while (1) // Loop trying to connect until connect
    {
        *client_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (*client_fd < 0)
        {
#ifdef DEBUG
            perror("[receiver] Error creating socket");
#endif
            return -1;
        }

#ifdef DEBUG
        printf("[receiver] Trying to connect to %s:%d...\n", server_host,
               SERVER_PORT);
#endif

        if (connect(*client_fd, (struct sockaddr *)server_address,
                    sizeof(*server_address)) == 0)
        {
#ifdef DEBUG
            printf("[receiver] Connected to %s:%d\n", server_host, SERVER_PORT);
#endif
            return 0; // Exit from loop
        }

#ifdef DEBUG
        perror("[receiver] Connection failed");
        printf("[receiver] Retrying in 2 seconds...\n");
#endif

        close(*client_fd);
        sleep(2);
    }
}

ssize_t recv_server_command(int client_fd, uint8_t *buffer,
                            size_t buffer_size)
{
    ssize_t total_received = 0, bytes_received;

    if (buffer == NULL || buffer_size == 0)
    {
        errno = EINVAL;
        return -1;
    }

    memset(buffer, 0, buffer_size);

    while (total_received < buffer_size - 1)
    {
        bytes_received = recv(client_fd, (uint8_t *)buffer + total_received,
                              buffer_size - total_received - 1, 0);
        if (bytes_received < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
#ifdef DEBUG
            perror("[receiver] Error receiving response");
#endif
            return -1;
        }
        if (bytes_received == 0)
        {
#ifdef DEBUG
            printf("[receiver] Connection closed by server\n");
#endif
            return -1;
        }
        total_received += bytes_received;

        if (buffer[total_received - 1] == '\n')
        {
            break;
        }
    }

    buffer[total_received] = '\0';

    return total_received;
}
