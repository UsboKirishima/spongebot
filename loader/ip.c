#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define IP(a, b, c, d) #a "." #b "." #c "." #d

typedef ip_t int8_t;


static _Bool is_host_up(const char *ip, int port) {
    int sock;
    struct sockaddr_in server;

    if(port != 22)
        printf("[INFO] Host selected does not uses default ssh port (22)\n");

    sock = socket(AF_INET, SOCK_STREAM, 0);
    
    if (sock == -1)
        return 0;

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(ip);

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == 0) {
        close(sock);
        return 1; 
    }

    close(sock);
    return 0;
}
