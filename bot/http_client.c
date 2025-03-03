#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <stdio.h>

#define SERVER_PORT 8080
#define BUFFER_SIZE 4096 

int connection_init(int *client_fd, struct sockaddr_in *server_address, const char *server_host) {
    struct hostent *server;

    *client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (*client_fd < 0) {
#ifdef DEBUG
        perror("[http] Error creating socket");
#endif
        return -1;
    }

    server = gethostbyname(server_host);
    if (server == NULL) {
#ifdef DEBUG
        fprintf(stderr, "[http] No such host: %s\n", server_host);
#endif
        return -1;
    }

    memset(server_address, 0, sizeof(*server_address));
    server_address->sin_family = AF_INET;
    server_address->sin_port = htons(SERVER_PORT);
    memcpy(&server_address->sin_addr.s_addr, server->h_addr_list[0], server->h_length);

    if (connect(*client_fd, (struct sockaddr *)server_address, sizeof(*server_address)) < 0) {
#ifdef DEBUG
        perror("[http] Connection failed");
#endif
        return -1;
    }

#ifdef DEBUG
    printf("[http] Connected to %s:%d\n", server_host, SERVER_PORT);
#endif
    return 0;
}

ssize_t send_http_request(int client_fd, const char *host, const char *path) {
    char request[BUFFER_SIZE];

    //TODO: each request needs to be saved on dictionary
    snprintf(request, sizeof(request),
             "GET %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "User-Agent: Crabby Patty/1.0\r\n"
             "Connection: close\r\n"
             "\r\n",
             path, host);

#ifdef DEBUG
    printf("[http] Sending request:\n%s", request);
#endif

    return send(client_fd, request, strlen(request), 0);
}

ssize_t recv_http_response(int client_fd, char *buffer, size_t buffer_size) {
    ssize_t total_received = 0, bytes_received;

    memset(buffer, 0, buffer_size);

    while ((bytes_received = recv(client_fd, buffer + total_received, buffer_size - total_received - 1, 0)) > 0) {
        total_received += bytes_received;
    }

    if (bytes_received < 0) {
#ifdef DEBUG
        perror("[http] Error receiving response");
#endif
        return -1;
    }

    return total_received;
}

