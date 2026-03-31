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
    rio_t rio; int err_gestion ; 

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
    if(clientfd < 0){
        printf("Erreur de connexion \n") ;
        exit(0) ; 
    }

    /*
        * At this stage, the connection is established between the client
        * and the server OS ... but it is possible that the server application
        * has not yet called "Accept" for this connection
    */
    printf("client connected to server OS\n"); 
    
    Rio_readinitb(&rio, clientfd);
    printf("ftp > ");

    // Gestion de la reponse de connexion au serveur maitre  
    reponse_t rep ; 
    if(rio_readnb(&rio, &rep, sizeof(reponse_t))<=0){
        printf("ERREUR ! Serveur ne repond pas \n ");
        exit(0);
    }

    if(ntohs(rep.code_retour)==67){
        printf("Connexion impossible , reesayer plus tard \n") ; 
        Close(clientfd) ;
        exit(0) ; 
    }

    else if(ntohs(rep.code_retour)==42){
        int new_port = ntohs(rep.info) ; // lecture du port du serveur esclave

        if(rio_readnb(&rio, &rep, sizeof(reponse_t))<=0){
        printf("ERREUR ! Serveur ne repond pas \n ");
        exit(0);
        }

        int new_host = ntohs(rep.info) ; // lecture du host du serveur esclave
        char new_host_name[50]; 
        sprintf(new_host_name,"%d", new_host) ; 
        clientfd = Open_clientfd(new_host_name, new_port); // nouvelle connexion etablie
        printf("Client est redirigé vers %s %d\n", new_host_name, new_port);
        Rio_readinitb(&rio, clientfd); // reinitialisation du buffer sur le nouveau canal de communication
    }
    else{
        printf("Erreur cote serveur \n") ; 
        Close(clientfd) ; 
        exit(0) ; 
    }

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

            // Reponse a la requette traite par la fonctiion gestion
            reponse_nb_bloc rep ; 
            if(rio_readnb(&rio, &rep, sizeof(reponse_t))<=0){
                printf("ERREUR ! Serveur ne repond pas \n ");
                printf("ftp > ");
                continue;
            }


            switch(ntohs(req.type)){
                case GET: 
                    err_gestion = gestion_get(rep,req.nom,req.offset,&rio);
                    if(err_gestion==-1){
                        printf("ECHEC! Serveur ne possede pas le fichier %s demandé \n", req.nom) ; 
                    }
                    else if(err_gestion==-2){
                        printf("ECHEC! Erreur lors de l'ouverture du fichier de côté serveur \n") ; 
                    }
                    else if(err_gestion!=0){
                        printf("ECHEC ! Erreur inconnu du côté de serveur \n");
                    }
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
        printf("ftp > ");
    }
    Close(clientfd);
    exit(0);
}
