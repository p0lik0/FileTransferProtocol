#include "type_req.h"
#include "gestion_client.h"

void gestion_get(reponse_t rep , char *nom , rio_t *rio){
    int code_retour = ntohs(rep.code_retour) ; 
    if(code_retour == 0){
        char nom_progress[100];
        strcpy(nom_progress, nom);
        strcat(nom, ".progress");

        struct stat file_status; 
        int offset = 0;
        if (stat(nom_progress,&file_status)>0){
            offset = file_status.st_size;
        }
        else if(errno!=ENOENT){
            perror("Erreur de stat() sur le fichier");
        }

        // ouverture du fichier
        int fd_file = open(nom_progress, O_WRONLY| O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR) ;
        lseek(fd_file,offset,SEEK_SET);
        if(fd_file == -1){
            perror("Errer lors de la création du fichier \n") ; 
        }
        
        int taille_contenu = ntohs(rep.taille_contenu) ; 
        int nb_blocs = (taille_contenu % TAILLE_BUFFER ==0)? 
                        (taille_contenu /TAILLE_BUFFER ): ((taille_contenu / TAILLE_BUFFER)+1); // nombre de blocs

        int count_bloc = 0 ; bloc b ; 
        
        while(count_bloc < nb_blocs){
            Rio_readnb(rio,&b,sizeof(bloc));
            write(fd_file,b.buffer_bloc,ntohs(b.taille_bloc)) ; // ecrit le contenu du bloc avec la taille donne par le serveur
            count_bloc ++ ; 
        } // lecture du bloc

        close(fd_file) ;
        if(rename(nom_progress, nom)<0){
            perror("rename() :");
        }
        printf("Transfer successful completed \n ");
        printf("File %s created \n",nom) ; 
        printf("\n");
    }
    else{
        switch (rep.code_retour)
        {
        case -1:
            printf("Le fichier %s demandé n'existe pas \n", nom) ; 
            break;

        case 1:
            printf("Erreur lors de l'ouverture du fichier \n") ; 
            break;
        
        default:
            break;
        }
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
                if (stat(nom_progress,&file_status)<0 && errno==ENOENT){ // si le fichier de ce nom avec extention .progress n'existe pas
                    req->offset = 0;
                    strcpy(req->nom,argument) ; // copie le nom du fichier demande ou envoye
                    return 0 ; 
                }
                else if(errno!=ENOENT){
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
