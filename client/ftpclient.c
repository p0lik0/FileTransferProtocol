/*
 * echoclient.c - An echo client
 */
#include "csapp.h"
#define PORT 2121

int main(int argc, char **argv)
{
    int clientfd, port;
    char *host, buf[MAXLINE];
    rio_t rio;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <host> \n", argv[0]);
        exit(0);
    }
    host = argv[1];
    port = PORT;

    clientfd = Open_clientfd(host, port);
    
    printf("client connected to server OS\n"); 
    
    Rio_readinitb(&rio, clientfd);
    Fgets(buf, MAXLINE, stdin);



    Rio_writen(clientfd, buf, strlen(buf));
    while (Rio_readlineb(&rio, buf, MAXLINE) > 0) {
        Fputs(buf, stdout);
    }
    Close(clientfd);
    exit(0);
}
