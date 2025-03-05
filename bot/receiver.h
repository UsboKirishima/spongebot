#pragma once

int receiver_init(int *, struct sockaddr_in *, const char *);
ssize_t recv_server_command(int , char *, size_t );