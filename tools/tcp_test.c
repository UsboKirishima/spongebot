#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#define PORT 8080

int main(int argc, char *argv[])
{
    int sockfd, n;
    char recvBuff[1024];
    struct sockaddr_in serv_addr;
    struct hostent *hp = gethostbyname("localhost");

    memset(recvBuff, 0, sizeof(recvBuff));

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Error: Could not create socket");
        return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Error: Connect Failed");
        close(sockfd);
        return 1;
    }

    while (1)
    {
        const char buffer_to_send[42] = "Hello from bot\n";

        if (send(sockfd, buffer_to_send, strlen(buffer_to_send), 0) < 0)
        {
            perror("Error: Sending data");
            close(sockfd);
            return 1;
        }


        sleep(5);
    }
    while ((n = read(sockfd, recvBuff, sizeof(recvBuff) - 1)) > 0)
    {
        recvBuff[n] = '\0';
        if (fputs(recvBuff, stdout) == EOF)
        {
            perror("Error: fputs error");
            close(sockfd);
            return 1;
        }
    }

    if (n < 0)
    {
        perror("Error: Read error");
    }

    close(sockfd);
    return 0;
}
