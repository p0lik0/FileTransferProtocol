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
    int info ; 
} reponse_t ; 

typedef struct bloc{
    int taille_bloc ; 
    char buffer_bloc[TAILLE_BUFFER] ; 
} bloc ; 

typedef struct reponse_nb_bloc{ // structure passe au serveur lors de la reponse pour obtenir le nombre de bloc
    int code_retour ; // code du retour concernant le fichier 
    int32_t valeur ; // nombre de blocs envoye par le serveur
} reponse_nb_bloc ; 

#endif 
