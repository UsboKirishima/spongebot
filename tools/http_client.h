#pragma once

#include <netinet/in.h>

int connection_init(int *, struct sockaddr_in *, const char *);
ssize_t send_http_request(int, const char *, const char *);
ssize_t recv_http_response(int, char *, size_t);

