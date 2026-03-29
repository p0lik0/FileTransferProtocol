#include "type_req.h"
#include "gestion_client.h"

int gestion_get(reponse_t rep , char *nom , int offset, rio_t *rio){
    int code_retour = ntohs(rep.code_retour) ; 
    if(code_retour == 0){
        char nom_progress[100];
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

        int taille_contenu = ntohs(rep.taille_contenu) ; 
        // printf("taille_contenu : %d\n", taille_contenu);

        int nb_blocs = (taille_contenu % TAILLE_BUFFER ==0)? 
                        (taille_contenu /TAILLE_BUFFER ): ((taille_contenu / TAILLE_BUFFER)+1); // nombre de blocs

        int count_bloc = 0 ; bloc b ; 
        // printf("nb_blocs : %d\n", nb_blocs);
        while(count_bloc < nb_blocs){
            Rio_readnb(rio,&b,sizeof(bloc));
            write(fd_file,b.buffer_bloc,ntohs(b.taille_bloc)) ; // ecrit le contenu du bloc avec la taille donne par le serveur
            count_bloc ++ ; 

            // decommenter pour avoir du temps à tapper "ctrl+C" pour immiter une panne du côté de client
            if(count_bloc==5){
                sleep(4);
            }

        } // lecture du bloc

        close(fd_file) ;
        rename(nom_progress, nom);
        printf("Transfer successful completed \n ");
        printf("File %s created \n",nom) ; 
        printf("\n");
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
        else if(strcmp(commande,"ls")==0){
            req->type = htons(LS) ; 
            return 0 ; 
        }
        else if(strcmp(commande , "bye")==0){
            req->type = htons(CLOSE) ; 
            return 0 ; 
        }
        else{
            //printf("Commande inconue \n") ; 
            return -2 ; 
        }

}
