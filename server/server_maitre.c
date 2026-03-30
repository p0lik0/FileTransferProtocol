/*
 * serverp.c - A pool type server
 */

#include "csapp.h"
#include "type_req.h"
#include "service.h"

#define MAX_NAME_LEN 256
#define PORT 2121
#define NB_SLAVES 5// Taille du pool (nombre de fils)


int main(){

    Signal(SIGPIPE,SIG_IGN) ;    // S'il y'a une erreur de connexion on l'ignore

    int maitre_fd ; 
    int port_slave[NB_SLAVES] = {2122 , 2123 , 2124 , 2125 , 2126} ; // buffer des ports
    int host_slave[NB_SLAVES] = {0 , 0, 0, 0, 0} ; 
    int server_fd[NB_SLAVES];
    int slaves_dispo[NB_SLAVES] = {0,0,0,0,0} ; // occupe ou libres

    for(int i=0 ; i<NB_SLAVES ; i++){
        server_fd[i] = Open_clientfd(host_slave[i],port_slave[i]) ; //connexion maitre slave
        if(server_fd[i]<0) printf("Erreur de connexion \n") ; 


    }



    return 0 ; 
}