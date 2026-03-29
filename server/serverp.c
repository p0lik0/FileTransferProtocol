/*
 * serverp.c - A pool type server
 */

#include "csapp.h"
#include "type_req.h"
#include "service.h"

#define MAX_NAME_LEN 256
#define PORT 2121
#define NB_PROC 5// Taille du pool (nombre de fils)


//int tab_pid[NB_PROC] ; // table des pid des procs

/* Handler de signal de terminaison du server */
void handler_server(){

    // handler redefini sur le gestionnaire par defaut
    //signal(SIGINT, SIG_IGN); 

    printf("\n[Père %d] Arrêt programmé. Signal envoyé au pool...\n", getpid());
    kill(0, SIGINT);

    // wait(NULL) bloque tant qu'il reste au moins un fils en vie.
    while (wait(NULL) > 0); 

    printf("[Père %d] Tous les fils sont enterrés. Fin du serveur.\n", getpid());
    exit(0);
}


/* Proccedure TCP que tous les fils executent */
void child_main(int listenfd) {

    //signal(SIGINT, SIG_DFL); // on redefini le handler des fils pour le remttre sur celui du pere afin qu'ils oublient le handler du pere

    int connfd;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    clientlen = sizeof(clientaddr);

    while (1) {
        //printf("Boucle \n") ; 
        // TOUS les fils dorment ici. Le noyau en réveille un seul par client.
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        if (connfd < 0) {
            if (errno == EINTR)
                continue;
            perror("Accept error");
            continue;
        }
        printf("[Fils %d] Connexion avec le client établi\n", getpid());
        gestion(connfd); // appelle  a la fonction de gestion des services du serveur 
        //Close(connfd); La fermeture de la connexion se fera sur demande du client
    }
}

int main(int argc, char **argv)
{

     // Traitant du signal de terminaison chargé de fermer propement le serveur

    int listenfd, port;

    if (argc != 1) {
        fprintf(stderr, "usage: %s \n", argv[0]);
        exit(0);
    }
    port = PORT; // port d'ecoute predefini a 2121

    listenfd = Open_listenfd(port);

    // On crée le pool de fils AVANT d'accepter des connexions
    for (int i = 0; i < NB_PROC; i++) {
        if (Fork() == 0) { 
            // --- CODE DES FILS ---
            //tab_pid[i] = getpid() ; 
            child_main(listenfd); // Chaque fils part dans sa boucle infinie
            //exit(0); 
        }
    }
    Signal(SIGINT,handler_server) ;

    while (1){
        pause() ; // attente active pour maintenir le pere en vie
    }
    exit(0);
} 