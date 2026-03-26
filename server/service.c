#include "csapp.h"
#include "request.h"

void get_filename(int connfd, char *filename){
    size_t n;
    rio_t rio;

    Rio_readinitb(&rio, connfd);
    n = Rio_readlineb(&rio, filename, MAXLINE);

    if (n > 0)
        filename[strlen(filename)-1] = '\0'; // enleve '\n'
}

typereq_t get_request(int connfd, char * data){
    size_t n;
    rio_t rio;

    char line[MAXLINE];
    Rio_readinitb(&rio, connfd);
    int n = Rio_readlineb(&rio, line, MAXLINE);
    char cmd [4];
    strncpy(cmd, line, 3);
    cmd[3] = "\0";
    if(strcmp(cmd,"GET")==0){
        int l = strlen(line)-1;
        int c = 0; int i=3;
        while(i<strlen(line) && line[i]==' '){
            i++;
        }
        while(i<strlen(line) && line[i]!=' '){
            data[c]=line[i];
            i++;c++;
        }
        data[c]='\0';
        return GET;
    }
    else{
        return NONDEF;
    }
}