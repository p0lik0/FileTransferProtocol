/*
 * echo - read and echo text lines until client closes connection
 */
#include "csapp.h"
#include "type_req.h"

/* Fonction de gestion des services*/
void gestion(int connfd) {
    size_t n;
    int file_size;
    rio_t rio;
    Rio_readinit(&rio, connfd);
    request_t req; 

    // Reccuperation de la structure de la requette transmis par le client

    // lecture de la taille de la structure requette
    if(Rio_readnb(&rio,&req.taille,sizeof(int)) != sizeof(int)){
        echec(connfd,2) ; // Si la taille du premier champ de la requette ne correspond pas a un int echec
        return ; }

    // Lecture du reste la structure requette
    Rio_readnb(&rio , (char *)&req + sizeof(int) , req.taille - sizeof(int)) ; 


    /////////////////////////////////////////////////////////////////////////

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
    
    default:
        echec(connfd, 0) ; // Code de requette incorrect
        break;
    }
}

// Fonction d'echec qui renvois une reponse d'echec en precisant pourquoi , i le code d'erreur correspodant le type reste a definir voir dans le code les differents appels a echec et les parametre pour savoir a quoi correspond et quelle message envoye
void echec(int connfd , int i){

    reponse_t rep ; 

    rep.code_retour = i ; 
    *rep.contenu = NULL ; 
    *rep.nom = NULL ; 
    rep.taille = sizeof(reponse_t) ; 
    rep.taille_contenu = 0 ; 

    // Renvois du fichier par le canal de communication
    Rio_writen(connfd,&rep,sizeof(reponse_t)) ; 

    return ; 
}

// Fonction GET envois le fichier demande par le client ou une erreur s'il ne se trouve pas dans le rep
void get_file(int connfd , request_t req){

    reponse_t rep ; 


    int fd = open(req.nom,O_RDONLY) ; // ouverture du fichier demande

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
    rep.taille = sizeof(reponse_t) ; 

    // Si le fichier existe bien code de retour 0
    rep.code_retour = 0 ; 

    // Reaffecation du nom du fichier renvoye
    strncpy((char *)rep.nom, (char *)req.nom , 100) ; // copie le nom de la requette comme nom de la reponse

    // On nettois rep.contenu pour pas qu'il contienne des bits aleatoire qui n'ont rien a voir avec le contenu que va y ecrire 
    memset(rep.contenu,0,MAXLINE) ; 

    // Lecture du contenu du fichier passe en argument dans le champ contenu de reponse_t
    rio_t rio ; 
    Rio_readinitb(&rio,fd) ; 
    ssize_t n = Rio_readneb(&rio,rep.contenu,MAXLINE) ; 
    rep.taille_contenu = n ; 
    
    // Renvois du fichier par le canal de communication
    Rio_writen(connfd,&rep,sizeof(reponse_t)) ; 
    
    close(fd) ; 
}