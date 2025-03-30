/*
 * an FTP client
 */
 
#include "types.h"
#include "csapp.h"
#include "log.h"
#include "utils.h"
#include <time.h>

#define BUFFER_SIZE 4096
int global_clientfd;
log_t global_log;
response_t res = {0};


log_t fill_log(request_t req, response_t res){
	log_t log;
	log.request = req.request;
	strcpy(log.filename, req.filename);
	log.filesize = res.filesize;
	return log;
}	


void handler_iencli(int sig) {
	printf("Déconnexion brusque du client\n");
	printf("status %d\n",res.status);
	if (res.status == 0){
		close(global_clientfd);
		exit(0);
	}
	FILE * log;
	if ((log = fopen(".log", "w"))<0){
		printf("client : cannot save current data file\n");
		exit(1);
	}

	fprintf(log, "%d\n", global_log.request);
	fprintf(log, "%s\n", global_log.filename);
	fprintf(log, "%d\n", global_log.filesize);
	fprintf(log, "%d\n", global_log.paquets_recus);

	fclose(log);

	sleep(1);
	close(global_clientfd);
    exit(0);
}

/*convertit le type de la requetes 'get', 'put', 'ls' en type enuméré*/
int type_request(const char *str) {
    if (strcmp(str, "get") == 0) return GET;
    if (strcmp(str, "put") == 0) return PUT;
    if (strcmp(str, "ls") == 0) return LS;
    if (strcmp(str, "bye") == 0) return BYE;
	else printf("client : requête inconnue\n"); return UNKNOWN;
}

void isbyecli(request_t req, int clientfd) {
	if (req.request == BYE) {
    	Rio_writen(clientfd, &(req.request), sizeof(req.request)); //envoi du 'bye' au serveur
		printf("client : fermeture de la socket\n");
		Close(clientfd);
		exit(0);
	}
}

void client2server(int fd, int clientfd, int remaining, int paquets, log_t log){
	char buffer[BUFFER_SIZE];
	ssize_t n;
	while (BUFFER_SIZE-remaining < 0){
		global_log = log;
		if ((n = Rio_readn(clientfd, buffer, BUFFER_SIZE))>0) {
			Rio_writen(fd, buffer, n);
			paquets++;
			remaining = remaining - n;
			printf("client : paquet (%d) reçu de taille (%d), %d octets restants\n",paquets,n,remaining);
			log.paquets_recus = paquets; 
			/**/
			printf("affichage log\n");
		fprintf(stdout, "%d\n", global_log.request);
		fprintf(stdout, "%s\n", global_log.filename);
		fprintf(stdout, "%d\n", global_log.filesize);
		fprintf(stdout, "%d\n", global_log.paquets_recus);
		usleep(500000);		//0.5 seconde
		}
	}
		if ((n = Rio_readn(clientfd, buffer, remaining))>0) {
			Rio_writen(fd, buffer, n);
			paquets++;
			remaining = remaining - n;
			printf("client : paquet (%d) reçu de taille (%d), %d octets restants\n",paquets,n,remaining);
		}
}

int main(int argc, char **argv)
{
	Signal(SIGINT, handler_iencli);
    int clientfd; 
	int port;
    char host[10];
	//char pathclient[100] = "./Client/";

    if (argc != 1) {
        fprintf(stderr, "usage: %s\n", argv[0]);
        exit(1);
    }
	
    strcpy(host, "localhost");
    port = 2121;

    /*
     * Note that the 'host' can be a name or an IP address.
     * If necessary, Open_clientfd will perform the name resolution
     * to obtain the IP address.
     */
    if ((clientfd = Open_clientfd(host,port))<0)
	{
		perror("Open error\n");
		exit(1);
	}
    
	global_clientfd = clientfd;
    /*
     * At this stage, the connection is established between the client
     * and the server OS ... but it is possible that the server application
     * has not yet called "Accept" for this connection
     */
    printf("client : Client connected to server OS\n"); 
	printf("\n");

/*debut de la conversation avec le serveur*/
while(1) {
    request_t req = {0};
    //response_t res = {0};
	clock_t debut, fin;
	double tempsEcoule, debit;
	
	if (existslog() == true) {
		log_t log = readlog();
		
		int fd;
		if ((fd = open(clipath(log.filename), O_WRONLY, 0444)) > 0) {
			int deja_rempli = BUFFER_SIZE*log.paquets_recus;
			int remaining = log.filesize - deja_rempli;
			int paquets = log.paquets_recus;
			printf("client : reprise du transfert\n");
			lseek(fd, deja_rempli, SEEK_SET);
			debut = clock();
        	client2server(fd, clientfd, remaining, paquets, log);
			fin = clock();
			tempsEcoule = (double) (fin - debut)/CLOCKS_PER_SEC; // Pour avoir le temps en seconde
			debit = (double) (remaining/1000) / tempsEcoule;
			//printf("temps écoulé %.3f\n", temps);
			printf("client : transfert effectué avec un debit de %.2f KBytes/s\n", debit);
			remove("./.log");

        }
		continue;
	}

	/*filling the request structure by reading a line*/
	printf("ftp> ");
	char input[266] = {0};
	fgets(input, 266, stdin);
	char typereq[10] = {0};
	sscanf(input, "%s %s",typereq, &(req.filename));
	req.request = type_request(typereq);

	/*traiter le cas du 'bye' a part*/
	isbyecli(req, clientfd);

	/*gestion des cas limites*/
	if (req.request == UNKNOWN) continue;
	
	if (strlen(req.filename) == 0) {
		printf("client : entrez un fichier\n");
		continue;
	}


	/* sending the type of the request : GET | PUT | LS and the filename to the server */
    Rio_writen(clientfd, &(req), sizeof(req));
	
	/* fetching the response from the server */
    Read(clientfd, &res, sizeof(res));

	/*status manager*/
    if (res.status == NOT_FOUND){
        printf("client : erreur 404, fichier introuvable\n");
    }
    else if (res.status == FOUND){
		printf("client : le fichier requêté existe\n");

        int fd = Open(clipath(req.filename), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd<0) {
            perror("client : Open error");
            exit(1);
        }
		
		/*reading the file on the tube*/
        Read(clientfd, &res, sizeof(res));
		int remaining = res.filesize;
        int paquets = 0;
		log_t log = fill_log(req, res);
		if (res.status == SENDING){
			printf("client : réception du fichier %s en cours de taille %d ...\n", req.filename, res.filesize);
			debut = clock();
        	client2server(fd, clientfd, remaining, paquets, log);
			fin = clock();
			tempsEcoule = (double) (fin - debut)/CLOCKS_PER_SEC; // Pour avoir le temps en seconde
			debit = (double) (remaining/1000) / tempsEcoule;
			printf("client : fin de la reception avec %.2f Kbytes/s de débit\n",debit);
        }    
		printf("\n");
    }


}
/*end of the while*/
Signal(SIGINT, handler_iencli);


}
