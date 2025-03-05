#define _GNU_SOURCE

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>

#define SERVER_PORT 8080
#define BUFFER_SIZE 4096 

int receiver_init(int *client_fd, struct sockaddr_in *server_address, const char *server_host) {
    struct addrinfo hints, *res;
    int status;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP

    status = getaddrinfo(server_host, NULL, &hints, &res);
    if (status != 0) {
#ifdef DEBUG
        fprintf(stderr, "[http] getaddrinfo error: %s\n", gai_strerror(status));
#endif
        return -1;
    }

    *client_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (*client_fd < 0) {
#ifdef DEBUG
        perror("[http] Error creating socket");
#endif
        freeaddrinfo(res);
        return -1;
    }

    memset(server_address, 0, sizeof(*server_address));
    server_address->sin_family = AF_INET;
    server_address->sin_port = htons(SERVER_PORT);
    memcpy(&server_address->sin_addr, &((struct sockaddr_in *)res->ai_addr)->sin_addr, sizeof(struct in_addr));

    freeaddrinfo(res);

    if (connect(*client_fd, (struct sockaddr *)server_address, sizeof(*server_address)) < 0) {
#ifdef DEBUG
        perror("[http] Connection failed");
#endif
        close(*client_fd);
        return -1;
    }

#ifdef DEBUG
    printf("[http] Connected to %s:%d\n", server_host, SERVER_PORT);
#endif
    return 0;
}

ssize_t recv_server_command(int client_fd, char *buffer, size_t buffer_size) {
    ssize_t total_received = 0, bytes_received;

    memset(buffer, 0, buffer_size);

    while (total_received < buffer_size - 1) {
        bytes_received = recv(client_fd, buffer + total_received, buffer_size - total_received - 1, 0);
        if (bytes_received < 0) {
#ifdef DEBUG
            perror("[http] Error receiving response");
#endif
            return -1;
        }
        if (bytes_received == 0) { 
#ifdef DEBUG
            printf("[http] Connection closed by server\n");
#endif
            break;
        }
        total_received += bytes_received;
    }

    return total_received;
}

