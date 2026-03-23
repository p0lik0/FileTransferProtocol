#ifndef TYPE_REQ_H
#define TYPE_REQ_H

/* Type énumeré des requettes possibles */
typedef enum typereq_t { GET , PUT , LS } typereq_t ; 

/* Structure de données des requettes envoyées par le client */
typedef struct request_t{
    int taille ; // taille entière de la structure
    int type ; // type de requette 
    unsigned char nom[100] ; // nom du fichier envoyé ou  demandé par le client 
    unsigned char contenu[MAXLINE] ; // contenu du fichier envoye par le client
} request_t ;

typedef struct reponse_t{
    int taille ; // taille de la structure pour que le serveur puisse la renvoyer
    int code_retour ; // code de retour de la requette
    unsigned char nom[100] ; // nom du fichier renvoye par le serveur
    unsigned char contenu[MAXLINE] ; // contenu renvoye par le serveur 
    ssize_t taille_contenu ; 
} reponse_t ; 

#endif 
