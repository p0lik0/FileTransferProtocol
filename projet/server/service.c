/*
 * service.c - permit to provide the service asked by client
 */
#include "csapp.h"
#include "type_req.h"


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

    reponse_t rep ; 

    char filename[110];
    strcpy(filename, "./server/");
    strcat(filename, req.nom);

    int fd = open(filename,O_RDONLY) ; // ouverture du fichier demande

    // Dans le cas ou une erreur se produit lors de l'ouverture du fichier
    if(fd == -1){
        if(errno == ENOENT){
            echec(connfd,-1) ; // le fichier n'existe pas
        }
        else{
            echec(connfd,1) ; // Erreur lors de l'ouverture du fichier
        }
        return ; 
    }


    // taille de la structure 
    // rep.taille = sizeof(reponse_t) ; 

    // Si le fichier existe bien code de retour 0
    rep.code_retour = 0 ; 

    // Reaffecation du nom du fichier renvoye
    strncpy((char *)rep.nom, (char *)req.nom , 100) ; // copie le nom de la requette comme nom de la reponse

    // On nettois rep.contenu pour pas qu'il contienne des bits aleatoire qui n'ont rien a voir avec le contenu que va y ecrire 
    memset(rep.contenu,0,MAXLINE) ; 

    // Lecture du contenu du fichier passe en argument dans le champ contenu de reponse_t
    ssize_t n = read(fd,rep.contenu,MAXLINE) ; 
    if(n<0){
        echec(connfd , 1); // erreur lors de l'ouverture du fichier
        return ; 
    }
    rep.taille_contenu = n ; 
    
    // Renvois du fichier par le canal de communication
    Rio_writen(connfd,&rep,sizeof(reponse_t)) ; 
    
    printf("[Fils %d] requêtte traité\n",getpid()) ; 
    close(fd) ; 
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


        // lecture de la taille de la requette
        // if(Rio_readnb(&rio,&req.taille,sizeof(int)) != sizeof(int)){
        //     echec(connfd,2) ; // Si la taille du premier champ de la requette ne correspond pas a un int echec
        //     return ; }

        // Lecture du reste la structure requette
        // Rio_readnb(&rio , (char *)&req + sizeof(int) , req.taille - sizeof(int)) ; 
        Rio_readnb(&rio , &req , sizeof(request_t));

        // On analyse le type de requette et en fonction on effectue le service correspondant
        switch (req.type)
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
            printf("Fermeture demandé par le client %d \n", getpid()) ; 
            close(connfd) ; // s'il y'a une demande de fin de connexion du client
            return ;

        default:
            // echec(connfd, 0) ; // Code de requette incorrect
            break;
        }
    }
}