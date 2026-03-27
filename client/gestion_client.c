#include "type_req.h"
#include "gestion_client.h"


void gestion_get(reponse_t rep , char *nom , rio_t *rio){
    if(rep.code_retour == 0){
        // ouverture du fichier
        int fd_file = open(nom, O_WRONLY| O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR) ;
        if(fd_file == -1){
            perror("Errer lors de la création du fichier \n") ; 
        }
        
        int nb_blocs = (rep.taille_contenu % TAILLE_BUFFER ==0)? 
                        (rep.taille_contenu /TAILLE_BUFFER ): ((rep.taille_contenu / TAILLE_BUFFER)+1); // nombre de blocs

        int count_bloc = 0 ; bloc b ; 

        while(count_bloc < nb_blocs){
            Rio_readnb(rio,&b,sizeof(bloc));
            write(fd_file,b.buffer_bloc,ntohs(b.taille_bloc)) ; // ecrid le contenu du bloc avec la taille donne par le serveur
            count_bloc ++ ; 
        } // lecture du bloc

        close(fd_file) ; 
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
            req->type = htons(GET) ; 
            if(n==2){ // copie le nom seulement s'il a été passé 
                strcpy(req->nom,argument) ; // copie le nom du fichier demande ou envoye
                return 0 ; 
            }
            else{
                return -1 ; 
            }
        }
        else if (strcmp(commande ,  "put")==0){
            req->type = htons(PUT) ;
            if(n==2){ // copie le nom seulement s'il a été passé 
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
