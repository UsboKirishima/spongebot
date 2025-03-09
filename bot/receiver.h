#pragma once

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>

int receiver_init(int *, struct sockaddr_in *, const char *);
ssize_t recv_server_command(int, char *, size_t);