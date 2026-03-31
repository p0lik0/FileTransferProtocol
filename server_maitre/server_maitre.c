/*
 * servermaitre.c - Serveur maître pour la redirection vers un pool d'esclaves
 */

#include "csapp.h"
#include "type_req.h"

#define MAX_NAME_LEN 256
#define NB_SLAVES 5
#define PORT_MAITRE 2121

int main() {
    int connfd, listenfd;
    int max_fd;
    int n;
    int slave_dispo_found;
    int last_slave = 0;
    
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    clientlen = sizeof(clientaddr);
    
    reponse_t rep;
    fd_set readfds;

    /* Configuration des esclaves */
    int port_slave[NB_SLAVES] = {2122, 2123, 2124, 2125, 2126};
    char host_slave[NB_SLAVES][10] = {"127.0.0.1", "127.0.0.1", "127.0.0.1", "127.0.0.1", "127.0.0.1"};
    int server_fd[NB_SLAVES];
    int slaves_dispo[NB_SLAVES] = {0, 0, 0, 0, 0};

    /* Initialisation des connexions avec les esclaves */
    for (int i = 0; i < NB_SLAVES; i++) {
        server_fd[i] = open_clientfd(host_slave[i], port_slave[i]);
        if (server_fd[i] < 0) {
            fprintf(stderr, "Échec connexion esclave %d sur port %d\n", i, port_slave[i]);
        } else {
            slaves_dispo[i] = 1; 
        }
    }

    listenfd = Open_listenfd(PORT_MAITRE);

    while (1) {
        /* Nettoyage et préparation de l'ensemble de lecture */
        FD_ZERO(&readfds);
        FD_SET(listenfd, &readfds);
        max_fd = listenfd;

        for (int i = 0; i < NB_SLAVES; i++) {
            if (server_fd[i] > 0) {
                FD_SET(server_fd[i], &readfds);
                if (server_fd[i] > max_fd) max_fd = server_fd[i];
            }
        }

        /* Attente d'un événement sur l'un des descripteurs */
        Select(max_fd + 1, &readfds, NULL, NULL, NULL);

        /* Analyse des mises à jour provenant des esclaves */
        for (int i = 0; i < NB_SLAVES; i++) {
            if (server_fd[i] > 0 && FD_ISSET(server_fd[i], &readfds)) {
                n = Read(server_fd[i], &rep, sizeof(reponse_t));
                if (n > 0) {
                    /* Mise à jour de l'état de disponibilité via le champ taille_contenu */
                    slaves_dispo[i] = ntohs(rep.info);
                } else if (n == 0) {
                    /* Gestion de la déconnexion d'un esclave */
                    Close(server_fd[i]);
                    server_fd[i] = -1;
                    slaves_dispo[i] = 0;
                }
            }
        }

        /* Gestion des nouvelles demandes de clients */
        if (FD_ISSET(listenfd, &readfds)) {
            connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);

            /* Recherche d'un esclave disponible (algorithme Round Robin) */
            slave_dispo_found = 0;
            for (int i = 0; i < NB_SLAVES; i++) {
                int target = (last_slave + 1 + i) % NB_SLAVES;
                if (slaves_dispo[target] == 1) {
                    last_slave = target;
                    slave_dispo_found = 1;
                    break;
                }
            }

            if (slave_dispo_found == 0) {
                /* Aucun esclave disponible : envoi code erreur 67 */
                memset(&rep, 0, sizeof(reponse_t));
                rep.code_retour = htons(67);
                rep.info = htons(0);
                Rio_writen(connfd, &rep, sizeof(reponse_t));
            } else {
                /* Esclave trouvé : transmission des coordonnées (Port et IP) */
                memset(&rep, 0, sizeof(reponse_t));
                rep.code_retour = htons(42);
                rep.info = htons(port_slave[last_slave]);
                Rio_writen(connfd, &rep, sizeof(reponse_t));

                /* Envoi de l'adresse IP (format simplifié pour test) */
                rep.code_retour = htons(42);
                rep.info = htons(0); 
                Rio_writen(connfd, &rep, sizeof(reponse_t));
            }
            
            /* Libération de la connexion client après redirection */
            Close(connfd);
        }
    }

    return 0 ; 
}