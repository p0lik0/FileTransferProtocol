/*
 * serverp.c - A pool type server
 */

#include "csapp.h"
#include "type_req.h"
#include "service.h"

#define MAX_NAME_LEN 256
//#define PORT 2121
#define NB_PROC 2// Taille du pool (nombre de fils)

int connfd2 ; 
reponse_t rep ; 

int nb_fils_occupe = 0 ; 
//int tab_pid[NB_PROC] ; // table des pid des procs

/* Handler de signal de terminaison du server */
void handler_server(){

    // handler redefini sur le gestionnaire par defaut
    signal(SIGINT, SIG_IGN); 

    printf("\n[Père %d] Arrêt programmé. Signal envoyé au pool...\n", getpid());
    kill(0, SIGINT);

    // wait(NULL) bloque tant qu'il reste au moins un fils en vie.
    while (wait(NULL) > 0); 

    printf("[Père %d] Tous les fils sont enterrés. Fin du serveur.\n", getpid());
    exit(0);
}

void handler_USR1(){
    nb_fils_occupe ++ ;
    if(nb_fils_occupe==NB_PROC){ // changement d'etat indisponible
        rep.code_retour = htons(69) ; 
        rep.info = htons(0) ; 
        Rio_writen(connfd2, &rep, sizeof(reponse_t));
    } 
}

void handler_USR2(){
    nb_fils_occupe -- ; 
    if(nb_fils_occupe==NB_PROC-1){ // changement d'etat le serveur devient disponible
        rep.code_retour = htons(69) ; 
        rep.info = htons(1) ; 
        Rio_writen(connfd2, &rep, sizeof(reponse_t)); 
    }
}


/* Proccedure TCP que tous les fils executent */
void child_main(int listenfd) {

    signal(SIGINT, SIG_DFL); // on redefini le handler des fils pour le remttre sur celui du pere afin qu'ils oublient le handler du pere

    int connfd;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    clientlen = sizeof(clientaddr);

    while (1) {
        //printf("Boucle \n") ; 
        // TOUS les fils dorment ici. Le noyau en réveille un seul par client.
        Signal(SIGUSR1,SIG_DFL) ;
        Signal(SIGUSR2,SIG_DFL) ;

        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        kill(getppid(),SIGUSR1) ; // signal de processus occupe
        
        printf("[Fils %d] Connexion avec le client établi\n", getpid());
        gestion(connfd); 
        kill(getppid(),SIGUSR2) ; // une fois sortie de gestion on est free
    }
}

int main(int argc, char **argv)
{
    Signal(SIGUSR1,handler_USR1) ; 
    Signal(SIGUSR2,handler_USR2) ; 

    Signal(SIGINT,handler_server) ;

    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    clientlen = sizeof(clientaddr);

    // Traitant du signal de terminaison chargé de fermer propement le serveur
    int listenfd, port;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <numero de port>\n", argv[0]);
        exit(0);
    }
    //port = PORT; // port d'ecoute predefini a 2121
    port = atoi(argv[1]) ; 

    listenfd = Open_listenfd(port);
    connfd2 = Accept(listenfd, (SA *)&clientaddr, &clientlen); // etabli la connexion avec le serveur maitre qui est un client pour lui
    printf("Connexion avec le serveur maitre termine \n") ; 

    // On crée le pool de fils AVANT d'accepter des connexions
    for (int i = 0; i < NB_PROC; i++) {
        if (Fork() == 0) { 
            // --- CODE DES FILS ---
            //tab_pid[i] = getpid() ; 
            child_main(listenfd); // Chaque fils part dans sa boucle infinie
            //exit(0); 
        }
    }

    //Signal(SIGINT,handler_server) ;

    while(1) // tres important
    pause() ; // attente active pour maintenir le pere en vie
    exit(0);
}