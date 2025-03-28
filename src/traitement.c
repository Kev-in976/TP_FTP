#include "types.h"
#include "csapp.h"
#include <stdlib.h>

#define BUFFER_SIZE 4096

void traitement(int connfd){

while(1) {	
    request_t req;
    response_t res;
    ssize_t n;
    int fd;
    char buffer [BUFFER_SIZE];

	/*filling the request structure*/
    if ((n = Rio_readn(connfd, &(req.request), sizeof(req.request)))<0){
		perror("Read error\n");
		exit(1);
	}
	
	if (req.request == BYE)
	{
		printf("traitement : fermeture de la socket par le client\n");
		Close(connfd);
		exit(0);
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
			printf("traitement : Requête de type GET pour le fichier %s\n",req.filename);

			if ((fd = Open(pathserver, O_RDONLY, 0444))<0){

				printf("traitement : fichier introuvable\n");
				res.status = 404;
				Rio_writen(connfd, &(res.status), sizeof(res.status));    //envoi du statut d'erreur au client
			}
			else 
			/*the file exists, so we proceed to the transfer*/
			{
				/*envoi du statut au client*/
				res.status = FOUND;
				Rio_writen(connfd, &(res.status), sizeof(res.status));

				/*we want to fetch the size of the file*/
				FILE *f = fopen(pathserver, "r");
				fseek(f, 0, SEEK_END); // seek to end of file
				int sizefd = ftell(f); // get current file pointer
				fseek(f, 0, SEEK_SET); // seek back to beginning of file
				fclose(f);
				
				/*envoi du statut et de la taille au client*/
                res.status = SENDING;
				res.filesize = sizefd;
                Rio_writen(connfd, &(res.status), sizeof(res.status));
                Rio_writen(connfd, &(res.filesize), sizeof(res.filesize));
				
				/*envoi du fichier par morceaux*/
				printf("traitement : envoi du fichier %s de taille %d en cours...\n", req.filename, res.filesize);
				int paquets = 0;
				int remaining = res.filesize;

				while (BUFFER_SIZE-remaining < 0){
					if ((n = Rio_readn(fd, buffer, BUFFER_SIZE))>0){
						Rio_writen(connfd, buffer, n);
                        paquets++;
                        remaining = remaining - n;
                        printf("traitement : paquet (%d) envoyé de taille (%zd), %d octets restants\n",paquets,n,remaining);
					}
               	}
                    if ((n = Rio_readn(fd, buffer, remaining))>0) {
                        Rio_writen(connfd, buffer, n);
                        paquets++;
                        remaining = remaining - n;
                        printf("traitement : paquet (%d) envoyé de taille (%zd), %d octets restants\n",paquets,n,remaining);

				}
				printf("traitement : file transfered\n");
				close(fd);
				}
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
	/*end of the switch*/

	}
	/*end of the loop (which is never reached)*/
}    
