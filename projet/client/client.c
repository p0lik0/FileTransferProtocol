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

        if(n==2){ // copie le nom seulement s'il a été passé 
            strcpy(req.nom,argument) ; // copie le nom du fichier demande ou envoye 
        }
        // req.taille = sizeof(request_t) ; // taille de la requette 


        // Rio_writen(clientfd, &req, req.taille); // ecriture de la requette dans le canal de communication
        Rio_writen(clientfd, &req, sizeof(request_t));

        // Gestion de la réponse du serveur
        reponse_t rep ; 
        // while (Rio_readnb(&rio, &rep, sizeof(reponse_t)) > 0) {
            Rio_readnb(&rio, &rep, sizeof(reponse_t));
            if(rep.code_retour == 0){
                printf("Transfer succesfully complete. \n") ; 
                printf("%ld bytes receives \n", sizeof(reponse_t)) ; 

                // creation du fichier recu du serveur
                char filename[110];
                strcpy(filename, "./client/");
                strcat(filename,rep.nom);

                int fd_file = open(filename, O_WRONLY| O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR) ;
                if(fd_file == -1){
                    perror("Errer lors de la création du fichier \n") ; 
                }
                else{
                    if(write(fd_file,rep.contenu,rep.taille_contenu) != rep.taille_contenu){
                        perror("Erreur lors de l'écriture dans le fichier \n") ; 
                    }
                    else{
                        printf("file %s succesfull create \n",rep.nom) ; 
                    }
                } 
                close(fd_file) ; 

                //printf("%s", rep.contenu) ; 
            }
            else{
                switch (rep.code_retour)
                {
                case -1:
                    printf("Le fichier %s demandé n'existe pas \n", rep.nom) ; 
                    break;

                case 1:
                    printf("Erreur lors de l'ouverture du fichier \n") ; 
                    break;

                case 2:
                    printf("La taille de la requette n'est pas un  int \n") ; 
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
