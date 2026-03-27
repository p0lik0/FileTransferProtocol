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
    rep.taille_contenu = 0; // Déjà à 0 grâce au memset, mais c'est plus clair

    // Pas besoin de toucher à rep.nom ou rep.contenu, ils sont déjà pleins de zéros
    
    Rio_writen(connfd, &rep, sizeof(reponse_t)); 
    printf("[Fils %d] requette traite\n", getpid()) ; 
}


// Fonction GET envois le fichier demande par le client ou une erreur s'il ne se trouve pas dans le rep
void get_file(int connfd , request_t req){

    Signal(SIGPIPE, SIG_IGN);

    struct stat file_status; 
    if (stat(req.nom,&file_status)<0){
        perror("Erreur de stat() sur le fichier");
    }
        
    reponse_t rep ; 

    int fd = open(req.nom,O_RDONLY) ; // ouverture du fichier demande

    // Dans le cas ou une err./servereur se produit lors de l'ouverture du fichier
    if(fd == -1){
        if(errno == ENOENT){
            echec(connfd,-1) ; // le fichier n'existe pas
        }
        else{
            echec(connfd,1) ; // Erreur lors de l'ouverture du fichier
        }
        return ; 
    }

    // Si le fichier existe bien code de retour 0
    rep.code_retour = 0 ; 

    rep.taille_contenu = htons(file_status.st_size); 
    
    // Renvois du code de retour et de la taille du fichier
    Rio_writen(connfd,&rep,sizeof(reponse_t)) ; 

    int n ; bloc b ; 

    int offset = ntohs(req.offset);
    if(offset>0){
        lseek(fd, offset, SEEK_SET);
    }

    while((n = read(fd,b.buffer_bloc,TAILLE_BUFFER))>0){
        b.taille_bloc = htons(n) ; // host to netxork conversion for endian type 
        if(rio_writen(connfd,&b,sizeof(bloc))<0){
            printf("[Fils %d] Connexion avec client interrompu\n", getpid());
            Close(fd);
            Close(connfd);
        }
    }
    
    printf("[Fils %d] requêtte traité\n",getpid()) ; 
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
        Rio_readnb(&rio , &req , sizeof(request_t));

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
            printf("[Fils %d] Fermeture demandé par le client\n", getpid()) ; 
            close(connfd) ; // s'il y'a une demande de fin de connexion du client
            return ;

        default:
            // echec(connfd, 0) ; // Code de requette incorrect
            break;
        }
    }
}