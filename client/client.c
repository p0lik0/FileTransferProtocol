/*
 * client.c - A c programm simulating a client in network communication
 */
#include "csapp.h"
#include "type_req.h"
#include "gestion_client.h"

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
    if(clientfd < 0) printf("Erreur de connexion \n") ; 
    
    /*
     * At this stage, the connection is established between the client
     * and the server OS ... but it is possible that the server application
     * has not yet called "Accept" for this connection
     */
    printf("client connected to server OS\n"); 
    
    Rio_readinitb(&rio, clientfd);

    while (Fgets(buf, MAXLINE, stdin) != NULL) {
        // Une fois que l'on reccupere la demande du client on cree la structure de la requette a envoyer

        /*                                                  ENVOIS DE LA REQUETTE                                                            */
        int n = sscanf(buf , "%s %s", commande, argument) ; 

        if(n<1){
            printf("Commande invalide , réesayer \n") ; 
            continue ; 
        }

        request_t req ;
        int r = compose_requete(commande, argument,&req,n);
        if(r==0){
            // Envoi de la requette dans le socket
            Rio_writen(clientfd, &req, sizeof(request_t)); 

            // Reponse gestion
            reponse_t rep ; 
            Rio_readnb(&rio, &rep, sizeof(reponse_t));

            switch(ntohs(req.type)){
                case GET:
                    gestion_get(rep,req.nom,&rio);
                    break;
                case CLOSE: 
                    Close(clientfd);
                    printf("Connexion closed \n") ; 
                    exit(0);
            }
        }
        else if(r==-1){
            printf("Absence d'arguments (nom de fichier)\n") ; 
        }
        else if(r==-3){
            perror("Erreur de stat() sur le fichier");
        }
        else if(r==-2){
            printf("Commande inconnue \n") ; 
        }

    }
    Close(clientfd);
    exit(0);
}
