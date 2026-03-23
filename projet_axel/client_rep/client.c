/*
 * echoclient.c - An echo client
 */
#include "csapp.h"
#include "type_req.h"


/* CODER LE CLIENT POUR QU'IL ENVOIS UN CONTENU DE TYPE REQUETTE EN FONCTION DE CE QUI EST RENTRE EN LIGNE DE
COMMANDE COMPARER LE PREMIER AVEC GET PUT OU LS ET LA SUITE PUIS CREER UNE STRUCTURE EN 
CONSEQUENCE A ENVOYER */

/* GERER LA STRUCTURE REPONSE POUR SAVOIR COMMENT INTERPRETTER LES REPONSES DU SERVEUR */
int main(int argc, char **argv)
{
    int clientfd, port;
    char *host, buf[MAXLINE];
    rio_t rio;

    char commande[50] = ""; // chaine contenant le type de requette ecrit sur l'entreee 
    char argument[100] = ""; // argument de la commande rentree

    if (argc != 3) {
        fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
        exit(0);
    }
    host = argv[1];
    port = atoi(argv[2]);

    /*
     * Note that the 'host' can be a name or an IP address.
     * If necessary, Open_clientfd will perform the name resolution
     * to obtain the IP address.
     */
    clientfd = Open_clientfd(host, port);
    
    /*
     * At this stage, the connection is established between the client
     * and the server OS ... but it is possible that the server application
     * has not yet called "Accept" for this connection
     */
    printf("client connected to server OS\n"); 
    
    Rio_readinitb(&rio, clientfd);

    while (Fgets(buf, MAXLINE, stdin) != NULL) {
        // Une fois que l'on reccupere la demande du client on cree l structure de requette pour creer la requette a envoyer
        int n = sscanf(buf , "%s %s", commande, argument) ; 

        if(n<1){
            printf("Commande invalide , réesayer \n") ; 
            continue ; 
        }

        request_t req; // initialise la commande a envoyée
        memset(&req, 0, sizeof(request_t)); // Sécurité : on met tout à zéro

        if(strcmp(commande, "get")==0){
            req.type = GET ; 
        }
        else if (strcmp(commande ,  "put")==0){
            req.type = PUT ;
        }
        else if(strcmp(commande,"ls")==0){
            req.type = LS ; 
        }
        else if(strcmp(commande , "close")==0){
            req.type = CLOSE ; 
        }
        else{
            printf("Commande inconue \n") ; 
        }

        if(n==2) // copie le nom seulement s'il a été passé 
        strcpy(req.nom,argument) ; // copie le nom du fichier demande ou envoye 
        req.taille = sizeof(request_t) ; // taille de la requette 



        Rio_writen(clientfd, &req, req.taille); // ecriture de la structure dans le canal de communication

        // Gestion de la réponse du serveur
        reponse_t rep ; 
        while (Rio_readnb(&rio, &rep, sizeof(reponse_t)) > 0) {
            if(rep.code_retour == 0){
                printf("Transfer succesfully complete. \n") ; 
                printf("%d bytes receives \n", sizeof(reponse_t)) ; 
                printf("%s", rep.contenu) ; 
            }
            else{
                switch (rep.code_retour)
                {
                case -1:
                    printf("Le fichier demandé n'existe pas \n") ; 
                    break;

                case 1:
                    printf("Erreur lors de l'ouverture du fichier \n") ; 
                    break;

                case 2:
                    printf("Requette de format inconnue \n") ; 
                    break;
                
                default:
                    break;
                }

            }

            //Fputs(buf, stdout);
        }
        /* else { /* the server has prematurely closed the connection 
            break;
        } */
    }
    Close(clientfd);
    exit(0);
}
