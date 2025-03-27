void traitementReq(int connfd){
    request_t req;
    response_t res;
    ssize_t n;
    int fd;
    char buffer [4096]; /*size will determined later*/
    ssize_t buff;
	
	/*filling the request structure*/
    if ((n = Rio_readn(connfd, &(req.request), sizeof(req.request)))<0){
		perror("Read error\n");
		exit(1);
	}
	
    if ((n = Rio_readn(connfd, &(req.filename), sizeof(req.filename)))<0){
		perror("Read error\n");
		exit(1);
	}
	
	/*filename formatting*/
	char pathserver[100] = "./Server/";
	strcat(pathserver, req.filename);
	
	/*request managing*/
	switch (req.request){
		case GET:
			printf("Requête de type GET pour le fichier %s\n",req.filename);

			if ((fd = Open(pathserver, O_RDONLY, 0444))<0){
				perror("fichier introuvable");
				res.status = 404;
				Rio_writen(connfd, &(res.status), sizeof(res.status));    //envoi de l'erreur au client
			}

			else 
			/*the file exists, so we proceed to the transfer*/
			{
				res.status = 200;
				Rio_writen(connfd, &(res.status), sizeof(res.status));    //envoi de l'erreur au client
				printf("Début du transfert du fichier %s...\n", req.filename);

				/*we want to charge the buffer in a row*/
				FILE *f = fopen(pathserver, "r");
				fseek(f, 0, SEEK_END); // seek to end of file
				int sizefd = ftell(f); // get current file pointer
				fseek(f, 0, SEEK_SET); // seek back to beginning of file
				fclose(f);
                int nb =1;
                printf("size = %d\n",sizefd);
				if (sizefd > sizeof(buffer)){
                    nb = (sizefd/sizeof(buffer)) + 1;
                }
                res.status = 100;
                Rio_writen(connfd, &(res.status), sizeof(res.status));

                printf("le fichier sera envoyé en %d paquets\n",nb);
				while((buff = Rio_readn(fd,buffer, sizeof(buffer)))>0){     //on check l'etat de read, si >0 on continue
					printf("Transfert en cours, %zd octets lu\n", buff);
					Rio_writen(connfd, buffer, buff);
				}
				printf("Ficher transfere avec succès\n");
			}
			close(fd);
			break;

		case PUT:
			printf("Requête de type PUT pour le fichier %s",req.filename);
			break;

		case LS:
			printf("Requête de type LS pour le fichier %s",req.filename);
			break;
		
		default:
			fprintf(stderr,"requete non reconnu\n");
			break;
	}
}    
