#ifndef SERVICE_H 
#define SERVICE_H

// Fonction de gestion des services du serveur
int gestion(int connfd);

// Fonction GET envois le fichier demande par le client ou une erreur s'il ne se trouve pas dans le rep
int get_file(int connfd , request_t req);

// Fonction d'echec qui renvois une reponse d'echec en precisant pourquoi , i le code d'erreur correspodant le type reste a definir voir dans le code les differents appels a echec et les parametre pour savoir a quoi correspond et quelle message envoye
void echec(int connfd , int i);

// Permet au serveur de recevoir un fichier envoye par le client
int put_file(int connfd, request_t req); 

// Renvois le contenu du repertoire courant du serveur
int do_ls(int connfd, request_t req); 

#endif