#include "type_req.h"
#include <time.h>
#include "gestion_client.h"

int gestion_get(reponse_nb_bloc rep , char *nom , int offset, rio_t *rio){
    int code_retour = ntohs(rep.code_retour) ; 
    if(code_retour == 0){
        struct stat st = {0};
        if (stat("./Downloads", &st) == -1) {
            mkdir("./Downloads", 0755);
        }
        char nom_progress[100];
        strcpy(nom_progress, "./Downloads/");
        strcpy(nom_progress, nom);
        strcat(nom_progress, ".progress");

        // ouverture du fichier .progress
        int fd_file = open(nom_progress, O_WRONLY| O_CREAT , S_IRUSR | S_IWUSR) ;
        if(fd_file == -1){
            perror("Errer lors de la création du fichier \n") ; 
            return -1;
        }
        offset = ntohs(offset);
        // printf("offset : %d\n ", offset);
        if(offset>0){ // si on est censé de completer un fichier qui a été deja telechargé en partie
            lseek(fd_file,offset,SEEK_SET); // deplacer la tete de lecture sur l offset
        }

        // on reccupere directement la taille du contenu dans le fichier
        int nb_blocs =  ntohl(rep.valeur) ; // nombre de blocs

        int count_bloc = 0 ; bloc b ; 
        int somme = 0 ;
        // printf("nb_blocs : %d\n", nb_blocs);
        clock_t start = clock();
        while(count_bloc < nb_blocs){
            Rio_readnb(rio,&b,sizeof(bloc));
            write(fd_file,b.buffer_bloc,ntohs(b.taille_bloc)) ; // ecrit le contenu du bloc avec la taille donne par le serveur
            count_bloc ++ ; 
            somme += ntohs(b.taille_bloc);

            // decommenter pour avoir du temps à tapper "ctrl+C" pour immiter une panne du côté de client
            // if(count_bloc==5){
            //     sleep(4);
            // }

        } // lecture du bloc
        clock_t end = clock();
        float seconds = (float)(end - start) / CLOCKS_PER_SEC;

        close(fd_file) ;
        char nom_down[100];
        strcpy(nom_down, "./Downloads/");
        strcat(nom_down, nom);
        rename(nom_progress, nom_down);
        printf("Transfer successful completed \n ");
        printf("File %s created \n",nom) ; 
        printf("%d bytes received in %f seconds (%f Kbytes/s).\n", somme, seconds, somme/(1024*seconds)) ; 
        // printf("\n");
        return 0;
    }
    else{
        return rep.code_retour;
    }
}


int compose_requete(char * commande, char *argument , request_t *req, int n){
        memset(req, 0, sizeof(request_t)); // Sécurité : on met tout à zéro

        if(strcmp(commande, "get")==0){
            if(n==2){ // copie le nom seulement s'il a été passé 
                req->type = htons(GET); 

                char nom_progress[100];
                strcpy(nom_progress, "Downloads/");
                strcpy(nom_progress, argument);
                strcat(nom_progress, ".progress");

                struct stat file_status; 
                if (stat(nom_progress,&file_status)!=0 && errno==ENOENT){ // si le fichier de ce nom avec extention .progress n'existe pas
                    req->offset = 0;
                    strcpy(req->nom,argument) ; // copie le nom du fichier demande ou envoye
                    return 0 ; 
                }
                else if(stat(nom_progress,&file_status)!=0 && errno!=ENOENT){
                    return -3;
                }
                else{ // <nom>.progress existe, cad le fichier à été déjà telechargé en partie
                    req->offset = htons(file_status.st_size);
                    strcpy(req->nom,argument) ; // copie le nom du fichier demande ou envoye
                    return 0 ; 
                }
            }
            else{
                return -1 ; 
            }
        }
        else if (strcmp(commande ,  "put")==0){
            if(n==2){ // copie le nom seulement s'il a été passé 
                req->type = htons(PUT) ;
                strcpy(req->nom,argument) ; // copie le nom du fichier demande ou envoye
                return 0 ; 
            }
            else{
                return -1 ; 
            }
        }
        // else if(strcmp(commande,"ls")==0){
        //     req->type = htons(LS) ; 
        //     return 0 ; 
        // }
        else if(strcmp(commande , "bye")==0){
            req->type = htons(CLOSE) ; 
            return 0 ; 
        }
        else{
            //printf("Commande inconue \n") ; 
            return -2 ; 
        }
}

void redirect_to_slave(int * clientfd, rio_t * rio, char *host, int port){
    *clientfd = Open_clientfd(host, port);
    if(*clientfd < 0){
        printf("Erreur de connexion \n") ;
        exit(0) ; 
    }

    printf("Client connected to server_maitre_FTP\n"); 
    
    Rio_readinitb(rio, *clientfd);
    //printf("ftp > ");
    //sleep(15) ; 
    reponse_t rep ; 
    if(rio_readnb(rio, &rep, sizeof(reponse_t))<=0){
        printf("ERREUR ! Le serveur maitre ne repond pas \n ");
        exit(0);
    }

    if(ntohs(rep.code_retour)==67){
        printf("Connexion impossible , reesayer plus tard \n") ; 
        Close(*clientfd) ;
        exit(0) ; 
    }

    else if(ntohs(rep.code_retour)==42){
        int new_port = ntohs(rep.info) ; // lecture du port du serveur esclave

        if(rio_readnb(rio, &rep, sizeof(reponse_t))<=0){
        printf("ERREUR ! Le serveur maitre ne repond pas \n ");
        exit(0);
        }

        int new_host = ntohs(rep.info) ; // lecture du host du serveur esclave
        char new_host_name[50]; 
        sprintf(new_host_name,"%d", new_host) ; 
        *clientfd = Open_clientfd(new_host_name, new_port); // nouvelle connexion etablie
        printf("Client est redirigé vers %s %d\n", new_host_name, new_port);
        printf("ftp > ");
        Rio_readinitb(rio, *clientfd); // reinitialisation du buffer sur le nouveau canal de communication
    }
    else{
        printf("Erreur cote serveur \n") ; 
        Close(*clientfd) ; 
        exit(0) ; 
    }
}
