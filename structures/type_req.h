#ifndef TYPE_REQ_H
#define TYPE_REQ_H
#define TAILLE_BUFFER 100

#include "csapp.h"

/* Type énumeré des requettes possibles */
typedef enum typereq_t { GET , PUT , LS , CLOSE } typereq_t ; 

/* Structure de données des requettes envoyées par le client */
typedef struct request_t{
    // int taille ; // taille entière de la structure
    int type ; // type de requette 
    char nom[100] ; // nom du fichier envoyé ou  demandé par le client 
    //char contenu[MAXLINE] ; // contenu du fichier envoye par le client
} request_t ;

typedef struct reponse_t{
    // int taille ; // taille de la structure pour que le serveur puisse la renvoyer
    int code_retour ; // code de retour de la requette
    //char nom[100] ; // nom du fichier renvoye par le serveur
    //char contenu[MAXLINE] ; // contenu renvoye par le serveur 
    ssize_t taille_contenu ; 
} reponse_t ; 

typedef struct bloc{
    int taille_bloc ; 
    char buffer_bloc[TAILLE_BUFFER] ; 
} bloc ; 

#endif 
