#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "http_client.h"
#include "ram_mem.h"

#define RESPONSE_SIZE 8192

int main(int argc, char **argv) {

    int client_fd;
    struct sockaddr_in server_address;
    char response[RESPONSE_SIZE];
    
    dict_init();

    if (connection_init(&client_fd, &server_address, dict_get(DICT_DOMAIN_NAME)) < 0) {
        return 1;
    }

    if (send_http_request(client_fd, dict_get(DICT_DOMAIN_NAME), "/users") < 0) {
        close(client_fd);
        return 1;
    }

    if (recv_http_response(client_fd, response, RESPONSE_SIZE) > 0) {
#ifdef DEBUG
        printf("\n[http] Server Response:\n%s\n", response);
#endif
    }

    
    close(client_fd);
    dict_free();
    return 0;
}

