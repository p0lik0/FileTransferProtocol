#include "csapp.h"

int get_filename(int connfd, char *filename){
    size_t n;
    rio_t rio;

    Rio_readinitb(&rio, connfd);
    n = Rio_readlineb(&rio, filename, MAXLINE);

    if (n <= 0)
        return -1;

    filename[strlen(filename)-1] = '\0'; // enlever '\n'

    int fd = open(filename, O_RDONLY, 0);

    return fd;
}