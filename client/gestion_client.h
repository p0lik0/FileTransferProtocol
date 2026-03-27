#ifndef GESTION_CLIENT_H
#define GESTION_CLIENT_H
#include "type_req.h"

void gestion_get(reponse_t rep , char *nom , rio_t *rio);

// Compose la requette a envoye pour serveur
int compose_requete(char * commande, char *argument , request_t *req, int n);


#endif