/*
 * echoserveri.c - An iterative echo server
 */

#include "csapp.h"

#define MAX_NAME_LEN 256
#define NPROC 5
#define PORT 2121

#include <signal.h>
#include "request.h"

int tab_pid[NPROC];

void handler(int sig) {
    for(int i = 0; i < NPROC; i++) {
        kill(tab_pid[i], SIGINT);
    }
    exit(0);
}

int get_filename(int connfd, char *filename);
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
    Signal(SIGINT, handler);

    for(int i=0; i<NPROC; i++){
        if((tab_pid[i]=Fork())==0) break;
    }

    while (1) {
        
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);

        /* determine the name of the client */
        Getnameinfo((SA *) &clientaddr, clientlen,
                    client_hostname, MAX_NAME_LEN, 0, 0, 0);
        
        /* determine the textual representation of the client's IP address */
        Inet_ntop(AF_INET, &clientaddr.sin_addr, client_ip_string,
                  INET_ADDRSTRLEN);
        
        printf("server connected to %s (%s)\n", client_hostname, client_ip_string);

        // Reception d'une requete
        char data[MAXLINE];
        typereq_t req =  get_request(connfd, data);
        switch(req){
            case GET:

                int fd = open(data,O_RDONLY,0);

                if(fd<0){
                    printf("File %s not found\n", data);
                    Close(connfd);
                    exit(0);
                }

                char buf[MAXLINE];
                ssize_t n;

                while ((n = read(fd, buf, MAXLINE)) > 0) {
                    Rio_writen(connfd, buf, n);
                }

                printf("File %s was sent\n",data);
                Close(fd);
                break;
            default:
                printf("Command is not found. \nUsage : GET <filename>\n");
        }

        Close(connfd);
    }
    exit(0);
}
