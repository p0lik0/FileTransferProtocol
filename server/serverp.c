/*
 * serverp.c - A pool type server
 */

#include "csapp.h"
#include "type_req.h"
#include "service.h"

#define MAX_NAME_LEN 256
#define PORT 2121
#define NB_PROC 5// Taille du pool (nombre de fils)


int tab_pid[NB_PROC] ; // table des pid des procs

/* Handler de signla de terminaison du server */
void handler_server(){

    for(int i=0 ; i<NB_PROC ; i++){
        Kill(tab_pid[i],SIGINT) ; // signal de terminaison envoyé au groupe pgid 
        printf("[Fils %d tue avec succes \n]",tab_pid[i]) ; 
    }
    
    exit(0) ; 
}


/* Proccedure TCP que tous les fils executent */
void child_main(int listenfd) {

    int connfd;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    clientlen = sizeof(clientaddr);

    while (1) {
        printf("Boucle \n") ; 
        // TOUS les fils dorment ici. Le noyau en réveille un seul par client.
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        printf("[Fils %d] Je traite une connexion\n", getpid());
        gestion(connfd); // appelle  a la fonction de gestion des services du serveur 
        //Close(connfd); La fermeture de la connexion se fera sur demande du client
    }
}

int main(int argc, char **argv)
{

    Signal(SIGINT,handler_server) ; // Traitant du signal de terminaison chargé de fermer propement le serveur

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
            tab_pid[i] = getpid() ; 
            child_main(listenfd); // Chaque fils part dans sa boucle infinie
            //exit(0); 
        }
    }

    char cmd[30];
    printf("Tapez 'close' pour arrêter le serveur proprement.\n");

    while (1) {
        if (fgets(cmd, sizeof(cmd), stdin) == NULL) break;
    
        if (strncmp(cmd, "close", 5) == 0) {
            break; 
        }
    }

    //Envoyer le signal au groupe (0 = tout mon groupe)
    printf("[Père] Envoi du signal d'arrêt aux fils...\n");
    kill(0, SIGINT); 

    // Attendre VRAIMENT tous les fils
    // wait(NULL) renvoie -1 quand il n'y a plus de fils à attendre
    while (wait(NULL) > 0); 

    printf("[Père] Tous les fils sont terminés. Fermeture.\n");
    exit(0);
} 