/*
 * service.c - permit to provide the service asked by client
 */
#include "csapp.h"
#include "type_req.h"
#include <sys/stat.h>
#include <time.h>


// Fonction d'echec qui renvois une reponse d'echec en precisant pourquoi , i le code d'erreur correspodant le type reste a definir voir dans le code les differents appels a echec et les parametre pour savoir a quoi correspond et quelle message envoye
void echec(int connfd, int i) {
    reponse_t rep; 

    // On met TOUTE la structure à zéro (champs, tableaux, tout)
    memset(&rep, 0, sizeof(reponse_t)); 

    // On remplit uniquement ce qui est nécessaire
    rep.code_retour = i; 
    // rep.taille = sizeof(reponse_t); 
    rep.info = 0; // Déjà à 0 grâce au memset, mais c'est plus clair

    // Pas besoin de toucher à rep.nom ou rep.contenu, ils sont déjà pleins de zéros
    
    Rio_writen(connfd, &rep, sizeof(reponse_t)); 
    printf("[Fils %d] echec de traitement de la requette\n", getpid()) ; 
}


// Fonction GET envois le fichier demande par le client ou une erreur s'il ne se trouve pas dans le rep
void get_file(int connfd , request_t req){

    Signal(SIGPIPE, SIG_IGN);
    char nom_down[100];
    strcpy(nom_down, "Files/");
    strcat(nom_down, req.nom);
    struct stat file_status; 
    if (stat(nom_down,&file_status)<0){
        perror("Erreur de stat() : ");
        echec(connfd,-1) ;
        return;
    }
        
    reponse_nb_bloc rep ; 

    int fd = open(nom_down,O_RDONLY) ; // ouverture du fichier demande

    // Dans le cas ou une err./servereur se produit lors de l'ouverture du fichier
    if(fd == -1){
        if(errno == ENOENT){
            echec(connfd,-1) ; // le fichier n'existe pas
        }
        else{
            echec(connfd,-2) ; // Erreur lors de l'ouverture du fichier
        }
        return ; 
    }

    // Si le fichier existe bien code de retour 0
    rep.code_retour = 0 ; 
    
    int offset = ntohs(req.offset);
    if(offset>0){
        // printf("rep.info :  %ld\n", file_status.st_size-offset);
        lseek(fd, offset, SEEK_SET);
        rep.valeur = ((file_status.st_size-offset) % TAILLE_BUFFER ==0)? ((file_status.st_size-offset)/TAILLE_BUFFER) : (((file_status.st_size-offset)/TAILLE_BUFFER)+1);
    }
    else{
        // printf("rep.info :  %ld\n", file_status.st_size);
        rep.valeur = (file_status.st_size % TAILLE_BUFFER ==0)? (file_status.st_size/TAILLE_BUFFER):((file_status.st_size/TAILLE_BUFFER)+1);
    }

    rep.valeur = htonl(rep.valeur) ; 
    // Renvois du code de retour et du nombre de bloc que le client doit recevoir
    Rio_writen(connfd,&rep,sizeof(reponse_t)) ; 

    int n ; bloc b ; 

    while((n = read(fd,b.buffer_bloc,TAILLE_BUFFER))>0){
        b.taille_bloc = htons(n) ; // host to netxork conversion for endian type 
        if(rio_writen(connfd,&b,sizeof(bloc))<0){
            printf("[Fils %d] Connexion avec client a été interrompu\n", getpid());
            Close(fd);
            Close(connfd);
        }
    }
    
    printf("[Fils %d] requêtte traité\n",getpid()) ; 
    // printf("nombre de bloc envoye %d \n",ntohl(rep.valeur)) ; 
    Close(fd) ; 
}

void put_file(int connfd , request_t req){
    return ; 
}

void do_ls(int connfd , request_t req){
    return ; 
}

/* Fonction de gestion des services*/
void gestion(int connfd) {

    rio_t rio;
    Rio_readinitb(&rio, connfd);
    request_t req; 

    // Reccuperation de la structure de la requette transmis par le client

    while(1){
        printf("[Fils %d] Attente d'une requête...\n", getpid()) ; 

        // Lecture du reste la structure requette
        if(rio_readnb(&rio , &req , sizeof(request_t))<=0){
            printf("[Fils %d] Connection avec le client a été interrompu (Panne côté client)\n", getpid()) ; 
            printf("Fermeture de la connexion \n") ; 
            close(connfd) ; // fermeture de la connexion
            return;
        }

        // On analyse le type de requette et en fonction on effectue le service correspondant
        switch (ntohs(req.type))
        {
        case GET:
            get_file(connfd , req) ; 
            break;

        case PUT:
            put_file(connfd, req) ; 
            break; 

        case LS:
            do_ls(connfd , req) ; 
            break ;

        case CLOSE:
            printf("[Fils %d] Connexion terminé par une demande de client\n", getpid()) ; 
            Close(connfd) ; // s'il y'a une demande de fin de connexion du client
            return ;

        default:
            // echec(connfd, 0) ; // Code de requette incorrect
            break;
        }
    }
}