#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>

#include "arp.h"

char *get_addr_by_name(const char *name){
    struct hostent *hst = NULL;
    if(name != NULL && (hst = gethostbyname(name)) != NULL)
        return inet_ntoa(*((struct in_addr*)hst->h_addr_list[0]));
    return NULL;
}

int wsa_init(void){
    WSADATA wsdata;
    if(WSAStartup(MAKEWORD(2, 2), &wsdata) != SOCKET_ERROR)
        return EXIT_SUCCESS;
    return SOCKET_ERROR;
}

void die(const char *msg){
    fprintf(stderr, msg);
    exit(0);
}

void help(void){
    printf("OPTIONS:\n\t");
    printf("-c\tNumber of requests sent\n\t");
    printf("-h\tUsing help\n");
    printf("EXAMPLES:\n\t");
    printf("arping 192.168.1.1\n\t");
    printf("arping -c 1 192.168.1.1\n");
}

int main(int argc, char **argv){
    LONGLONG count = EOF;
    int ch = 0;
    opterr = false;
    char *addr = NULL;
    wsa_init();
    for(ch = 0; ch < argc; ch++){
        if(argv[ch][0] == '-'){
            switch(argv[ch][1]){
                case 'c' :
                    count = atoi(argv[++ch]);
                    break;
                case 'h' :
                    help();
                    return 0;
                default :
                    die("Invalid argument\n");
            }
        }
        else
            addr = argv[ch];
    }
    if(argc > 1){
        if((addr = get_addr_by_name(addr)) != NULL)
            arping(addr, count);
        else
           die("Invalid IP address\n");
    }
    else
        help();
    WSACleanup();
    return 0;
}
