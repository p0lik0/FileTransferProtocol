/*
 * servermaitre.c - A master type server
 */

#include "csapp.h"
#include "type_req.h"

#define MAX_NAME_LEN 256
#define NB_SLAVES 5// Taille du pool (nombre de fils)
#define PORT_MAITRE 2121 // port du maitre 


int main(){

    int connfd;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    clientlen = sizeof(clientaddr);
    int n ; 

    int port_slave[NB_SLAVES] = {2122 , 2123 , 2124 , 2125 , 2126} ; // buffer des ports
    char host_slave[NB_SLAVES][10] = {"0" , "0", "0", "0", "0"} ; 
    int server_fd[NB_SLAVES];
    int slaves_dispo[NB_SLAVES] = {0,0,0,0,0} ; // tous sont occupés
    int last_slave = 0 ; // indice du dernier slave

    for(int i=0 ; i<NB_SLAVES ; i++){
        server_fd[i] = open_clientfd(host_slave[i],port_slave[i]) ; //connexion maitre slave
        if(server_fd[i]<0) printf("Erreur de connexion \n") ; 
        else
            slaves_dispo[i] = 1 ; // le serveur est disponible
    }

    int listenfd; int slave_dispo_found ; 
    reponse_t rep ; 

    listenfd = Open_listenfd(PORT_MAITRE) ; 

    //fd_set readfds; // ensemble de tous les descripteurs
    while(1){
        for(int i=0 ; i<NB_SLAVES ; i++){
            n = Read(server_fd[i], &rep, sizeof(reponse_t));
            if(n==sizeof(reponse_t)){
                slaves_dispo[i] = ntohs(rep.taille_contenu) ;
            }
        }

        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen) ; 

        slave_dispo_found = 0; 
        for(int i=last_slave+1 ; i<NB_SLAVES ; i++){
            if(slaves_dispo[i]==1){
                last_slave = i ; 
                slave_dispo_found = 1 ; 
                break ;
            }
        }

        if(slave_dispo_found==0){
            for(int j=0 ; j<last_slave+1 ; j++){
                if(slaves_dispo[j]==1){
                    last_slave = j ; 
                    slave_dispo_found = 1 ;
                    break ; 
                }
            }
        }
        if(slave_dispo_found ==0){
            rep.code_retour = htons(67) ; 
            rep.taille_contenu = htons(0) ; 
            Rio_writen(connfd, &rep, sizeof(reponse_t)); 
        }

        else{
            rep.code_retour = htons(42) ; 
            rep.taille_contenu = htons(port_slave[last_slave]); 
            Rio_writen(connfd, &rep, sizeof(reponse_t)); 

            rep.code_retour = htons(42); 
            rep.taille_contenu = atoi(host_slave[last_slave]); 
            Rio_writen(connfd, &rep, sizeof(reponse_t)); 
        }
    }

    return 0 ; 
}