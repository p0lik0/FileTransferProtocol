#ifndef TYPE_REQ_H
#define TYPE_REQ_H
#define TAILLE_BUFFER 100

#include "csapp.h"

/* Type énumeré des requettes possibles */
typedef enum typereq_t { GET , PUT , LS , CLOSE } typereq_t ; 

/* Structure de données des requettes envoyées par le client */
typedef struct request_t{
    int type ; // type de requette 
    char nom[100] ; // nom du fichier envoyé ou  demandé par le client 
    int offset ;
} request_t ;

typedef struct reponse_t{
    int code_retour ; // code de retour de la requette
    ssize_t taille_contenu ;
} reponse_t ; 

typedef struct bloc{
    int taille_bloc ; 
    char buffer_bloc[TAILLE_BUFFER] ; 
} bloc ; 

#endif 
