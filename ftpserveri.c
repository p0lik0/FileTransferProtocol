/*
 * echoserveri.c - An iterative echo server
 */

#include "csapp.h"

#define MAX_NAME_LEN 256
#define PORT 2121

#include <sys/stat.h> // stat

long get_file_size(char *filename) {
    struct stat file_status;
    if (stat(filename, &file_status) < 0) {
        return -1;
    }

    return file_status.st_size;
}

int get_filename(int connfd);

/* 
 * Note that this code only works with IPv4 addresses
 * (IPv6 is not supported)
 */
int main(int argc, char **argv)
{
    int listenfd, connfd, port;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    char client_ip_string[INET_ADDRSTRLEN];
    char client_hostname[MAX_NAME_LEN];
    
    port = PORT;
    
    clientlen = (socklen_t)sizeof(clientaddr);

    listenfd = Open_listenfd(port);
    connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);

    /* determine the name of the client */
    Getnameinfo((SA *) &clientaddr, clientlen,
                client_hostname, MAX_NAME_LEN, 0, 0, 0);
    
    /* determine the textual representation of the client's IP address */
    Inet_ntop(AF_INET, &clientaddr.sin_addr, client_ip_string,
                INET_ADDRSTRLEN);
    
    printf("server connected to %s (%s)\n", client_hostname,
            client_ip_string);

    int fd = get_filename(connfd);
    if(fd<0){
        printf("File not found\n");
        Close(connfd);
        exit(0);
    }

    char buf[MAXLINE];
    ssize_t n;

    while ((n = read(fd, buf, MAXLINE)) > 0) {
        Rio_writen(connfd, buf, n);
    }

    Close(fd);
    Close(connfd);
    exit(0);
}
