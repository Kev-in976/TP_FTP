#include "types.h"
#include "csapp.h"
#include <stdlib.h>
#include <stdbool.h>
#include "log.h"
#include "utils.h"
#define BUFFER_SIZE 4096


/*filling the request structure*/
request_t readreq(request_t req, int connfd) {	
    if ((Rio_readn(connfd, &req, sizeof(req)))<0){
		perror("Read error\n");
		exit(1);
	}
	return req;
}

bool isbyeserv(request_t req, int connfd) {
	if (req.request == BYE)
	{
		printf("traitement : fermeture de la socket par le client\n");
		return true;
		//Close(connfd);
		//exit(0);
	} return false;
}

/*bool existslog() {
	FILE *logfd;
	if ((logfd = fopen(".log", "r")) != NULL) {
		fclose(logfd);
		return true;
	} return false;
}

log_t readlog() {
	log_t log;
	FILE *f;
	if ((f = fopen(".log", "r")) != NULL) {
		fscanf(f, "%d", &log.request);
		fscanf(f, "%s", &log.filename);
		fscanf(f, "%d", &log.filesize);
		fscanf(f, "%d", &log.paquets_recus);
		return log;
	}
	else {
		printf("traitement : erreur d'ouverture de la log\n");
		exit(1);
	}
}
*/
void server2client(int fd, int connfd, int remaining, int paquets){
	char buffer[BUFFER_SIZE];
	ssize_t n;
	while (BUFFER_SIZE-remaining < 0){
		if ((n = Rio_readn(fd, buffer, BUFFER_SIZE))>0){
			Rio_writen(connfd, buffer, n);
			paquets++;
			remaining = remaining - n;
			printf("traitement : paquet (%d) envoyé de taille (%zd), %d octets restants\n",paquets,n,remaining);
		}
	}
	printf("la\n");
		if ((n = Rio_readn(fd, buffer, remaining))>0) {
			Rio_writen(connfd, buffer, n);
			paquets++;
			remaining = remaining - n;
			printf("traitement : paquet (%d) envoyé de taille (%zd), %d octets restants\n",paquets,n,remaining);

	}
}

/*char * filepath(request_t req) {
	char *path = malloc(sizeof(char)*256);
	printf("rq.filename :.%s.\n", req.filename);
	snprintf(path, 256, "./Server/%s", req.filename);	
	return path;
}

int catchsize(char *filepath){
	printf("filepath : .%s.\n", filepath);
	FILE *f;
	f = fopen(filepath, "r");
	fseek(f, 0, SEEK_END); // seek to end of file
	int sizefd = ftell(f); // get current file pointer
	fseek(f, 0, SEEK_SET); // seek back to beginning of file
	fclose(f);
	return sizefd;
}*/

void traitement(int connfd){
while(1) {	
    request_t req = {0};
    response_t res = {0};

	if (existslog()==true){

		log_t log = readlog();
		
		int fd; 
		if ((fd = open(servpath(log.filename), O_RDONLY, 0444)) > 0) {
			int deja_rempli = BUFFER_SIZE*log.paquets_recus;
			int remaining = log.filesize - deja_rempli;
			int paquets = log.paquets_recus;
			printf("traitement : reprise du transfert\n");
			lseek(fd, deja_rempli, SEEK_SET);
			server2client(fd,connfd, remaining, paquets);
			printf("traitement : transfert effectué");
			remove("./.log");
		}
		continue;

	}
				

	req = readreq(req, connfd);
	
	if ((isbyeserv(req, connfd)==true)){
		break;
	}
	
	

	/*request managing*/
	switch (req.request){
		case GET:
			printf("traitement : Requête de type GET pour le fichier %s\n",req.filename);
			int fd;
			printf("servpath = .%s.\n", servpath(req.filename));
			if ((fd = open(servpath(req.filename), O_RDONLY, 0444))<0){
				printf("traitement : fichier introuvable\n");
				res.status = NOT_FOUND;
				Rio_writen(connfd, &(res.status), sizeof(res.status));    //envoi du statut d'erreur au client
			}
			else 
			/*the file exists, so we proceed to the transfer*/
			{
				/*envoi du statut au client*/
				res.status = FOUND;
				Rio_writen(connfd, &(res), sizeof(res));

				/*we want to fetch the size of the file*/
				int sizefd = catchsize(servpath(req.filename));
				
				/*envoi du statut et de la taille au client*/
                res.status = SENDING;
				res.filesize = sizefd;
                Rio_writen(connfd, &(res), sizeof(res));
				
				/*envoi du fichier par morceaux*/
				int remaining = res.filesize;
				int paquets = 0;

				printf("traitement : envoi du fichier %s de taille %d en cours...\n", req.filename, res.filesize);
				server2client(fd,connfd, remaining,paquets);

				printf("traitement : file transfered\n");
				close(fd);
				res.status = 0;
				printf("status %d\n",res.status);
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
