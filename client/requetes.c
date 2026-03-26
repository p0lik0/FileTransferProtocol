void get_requete(){
    reponse_t rep ; 

        Rio_readnb(&rio, &rep, sizeof(reponse_t));
        if(rep.code_retour == 0){

            // ouverture du fichier
            int fd_file = open(req.nom, O_WRONLY| O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR) ;
            if(fd_file == -1){
                perror("Errer lors de la création du fichier \n") ; 
            }
            
            int nb_blocs = (rep.taille_contenu % TAILLE_BUFFER ==0)? 
                            (rep.taille_contenu /TAILLE_BUFFER ): ((rep.taille_contenu / TAILLE_BUFFER)+1); // nombre de blocs

            int count_bloc = 0 ; bloc b ; 

            while(count_bloc < nb_blocs){
                Rio_readnb(&rio,&b,sizeof(bloc));
                write(fd_file,b.buffer_bloc,ntohs(b.taille_bloc)) ; // ecrid le contenu du bloc avec la taille donne par le serveur
                count_bloc ++ ; 
            } // lecture du bloc

            close(fd_file) ; 
}

request_t compose_requete(char * command, char *argument){
    request_t req; // initialise la commande a envoyée
        memset(&req, 0, sizeof(request_t)); // Sécurité : on met tout à zéro

        if(strcmp(commande, "get")==0){
            req.type = htons(GET) ; 
        }
        else if (strcmp(commande ,  "put")==0){
            req.type = htons(PUT) ;
        }
        else if(strcmp(commande,"ls")==0){
            req.type = htons(LS) ; 
        }
        else if(strcmp(commande , "bye")==0){
            req.type = htons(CLOSE) ; 
        }
        else{
            printf("Commande inconue \n") ; 
        }

        if(n==2){ // copie le nom seulement s'il a été passé 
            strcpy(req.nom,argument) ; // copie le nom du fichier demande ou envoye 
        }

                
        printf("Transfer successful completed \n ");
        printf("File %s created \n",req.nom) ; 
        printf("\n");
}