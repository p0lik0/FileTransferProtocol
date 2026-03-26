/*
 * client.c - A c programm simulating a client in network communication
 */
#include "csapp.h"
#include "type_req.h"


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

        request_t req; // initialise la commande a envoyée
        memset(&req, 0, sizeof(request_t)); // Sécurité : on met tout à zéro

        if(strcmp(commande, "get")==0){
            req.type = htons(GET) ; 
        }
        else if (strcmp(commande ,  "put")==0){
            req.type = htons(PUT) ;
        }
        else if(strcmp(commande,"ls")==0){
            req.type = htons(LS) ; 
        }
        else if(strcmp(commande , "bye")==0){
            req.type = htons(CLOSE) ; 
        }
        else{
            printf("Commande inconue \n") ; 
        }

        if(n==2){ // copie le nom seulement s'il a été passé 
            strcpy(req.nom,argument) ; // copie le nom du fichier demande ou envoye 
        }
        // req.taille = sizeof(request_t) ; // taille de la requette 


        // Rio_writen(clientfd, &req, req.taille); // ecriture de la requette dans le canal de communication
        Rio_writen(clientfd, &req, sizeof(request_t)); 

        /*                                             GESTION DE LA REPONSE DU SERVEUR                                                  */
        reponse_t rep ; 

        Rio_readnb(&rio, &rep, sizeof(reponse_t));
        if(rep.code_retour == 0){

            // ouverture du fichier
            int fd_file = open(req.nom, O_WRONLY| O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR) ;
            if(fd_file == -1){
                perror("Errer lors de la création du fichier \n") ; 
            }
            
            int nb_blocs = (rep.taille_contenu % TAILLE_BUFFER ==0)? 
                            (rep.taille_contenu /TAILLE_BUFFER ): ((rep.taille_contenu / TAILLE_BUFFER)+1); // nombre de blocs

            int count_bloc = 0 ; bloc b ; 
            while(Rio_readnb(&rio,&b,sizeof(bloc)) && count_bloc < nb_blocs){
                write(fd_file,b.buffer_bloc,ntohs(b.taille_bloc)) ; // ecrid le contenu du bloc avec la taille donne par le serveur
                count_bloc ++ ; 
            } // lecture du bloc

            close(fd_file) ; 
            printf("Transfer successful completed \n ");
            printf("File %s created \n",req.nom) ; 

        }
        else{
            switch (rep.code_retour)
            {
            case -1:
                printf("Le fichier %s demandé n'existe pas \n", req.nom) ; 
                break;

            case 1:
                printf("Erreur lors de l'ouverture du fichier \n") ; 
                break;
            
            default:
                break;
            }
        }
            //Fputs(buf, stdout);
        // }
        /* else { // the server has prematurely closed the connection 
            break;
        } */
    }
    Close(clientfd);
    exit(0);
}
