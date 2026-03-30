/*
 * serverp.c - A pool type server
 */

#include "csapp.h"
#include "type_req.h"
#include "service.h"

#define MAX_NAME_LEN 256
#define NB_SLAVES 5// Taille du pool (nombre de fils)
#define PORT_MAITRE 2121 // port du maitre 


int main(){

    int connfd;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    clientlen = sizeof(clientaddr);

    int port_slave[NB_SLAVES] = {2122 , 2123 , 2124 , 2125 , 2126} ; // buffer des ports
    int host_slave[NB_SLAVES] = {0 , 0, 0, 0, 0} ; 
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
    while((connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen))){
        slave_dispo_found = 0; 
        for(int i=last_slave ; i<NB_SLAVES ; i++){
            if(slaves_dispo[i]==1){
                last_slave = i ; 
                slave_dispo_found = 1 ; 
                break ;
            }
        }

        if(slave_dispo_found==0){
            for(int j=0 ; j<last_slave ; j++){
                if(slaves_dispo[j]==1){
                    last_slave = j ; 
                    slave_dispo_found = 1 ;
                    break ; 
                }
            }
        }

        if(slave_dispo_found ==0){
            rep.code_de_retour = 67 ; 
            rep.taille_contenu = 0 ; 
        }
        else{
            
        }


    }


    return 0 ; 
}