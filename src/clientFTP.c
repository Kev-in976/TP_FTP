/*
 * an FTP client
 */
 
#include "types.h"
#include "csapp.h"

#define BUFFER_SIZE 4096

/*convertit le type de la requetes 'get', 'put', 'ls' en type enuméré*/
int type_request(const char *str) {
    if (strcmp(str, "get") == 0) return GET;
    if (strcmp(str, "put") == 0) return PUT;
    if (strcmp(str, "ls") == 0) return LS;
    if (strcmp(str, "bye") == 0) return BYE;
	else printf("client : requête inconnue\n"); return UNKNOWN;
}

/*INUTILE POUR l'INSTANT A VOIR SI CA SERT PLUS TARD*/
request_t parse_request(char *str) {
	str[strcspn(str, "\n")] = '\0';
	request_t *request = malloc(sizeof(request_t));
	char req[10];
	char file[256];
	int which = 0;
	char c;
	int i=0;
	int j = 0;
	while((str[i] != '\0')){ /*end of the string*/
		switch(c = str[i]){
			case ' ':
			req[i] = '\0';
			which = 1;

			default: /*it is a char*/
			if (which == 1) { /*parsing the filename*/
				printf("file[%d] = [%c]\n", j, c);
				file[j] = c;
				j++;
			} else { /*parsing the request type*/
				req[i] = c;
			}
		} /*switch end*/
		i++;
	} /*end while*/
	file[j] = '\0';
	request->request = type_request(req);
	strcpy(request->filename, file);
	printf("parse : str = [%s]\n", str);
	printf("parse : file = [%s]\n", file);
	printf("parse : request = [%s]\n", req);
	return *request;
}
/*INUTILE POUR l'INSTANT A VOIR SI CA SERT PLUS TARD*/


int main(int argc, char **argv)
{
    int clientfd, port;
    char host[10];
	char pathclient[100] = "./Client/";

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
    
    /*
     * At this stage, the connection is established between the client
     * and the server OS ... but it is possible that the server application
     * has not yet called "Accept" for this connection
     */
    printf("client : Client connected to server OS\n"); 
	printf("\n");

/*debut de la conversation avec le serveur*/
while(1) {
    request_t req;
    response_t res;
    char buffer [BUFFER_SIZE];
    int n;

	/*filling the request structure by reading a line*/
	printf("ftp> ");
	char input[266];
	fgets(input, 266, stdin);
	char typereq[10];
	sscanf(input, "%s %s",typereq, &(req.filename));

	req.request = type_request(typereq);

	/*traiter le cas du 'bye' a part*/
	if (req.request == BYE) {
    	Rio_writen(clientfd, &(req.request), sizeof(req.request)); //envoi du 'bye' au serveur
		printf("client : fermeture de la socket\n");
		Close(clientfd);
		exit(0);
	}

	/* sending the type of the request : GET | PUT | LS and the filename to the server */
    Rio_writen(clientfd, &(req), sizeof(req));
	
	/* fetching the response from the server */
    Read(clientfd, &res, sizeof(res));

	/*status manager*/
    if (res.status == NOT_FOUND){
        printf("client : erreur 404, fichier introuvable");
    }
    else if (res.status == FOUND){
		printf("client : le fichier requêté existe\n");

 		strcat(pathclient, req.filename);
        int fd = Open(pathclient, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd<0) {
            perror("client : Open error");
            exit(1);
        }
		
		/*reading the file on the tube*/
        Read(clientfd, &res, sizeof(res));
		printf("client : réception du fichier %s en cours de taille %d ...\n", req.filename, res.filesize);
		int remaining = res.filesize;
        int paquets = 0;
        
		if (res.status == SENDING){
			while (BUFFER_SIZE-remaining < 0){ 
            	if ((n = Rio_readn(clientfd, buffer, BUFFER_SIZE))>0) {
					Rio_writen(fd, buffer, n);
					paquets++;
					remaining = remaining - n;
					printf("client : paquet (%d) reçu de taille (%d), %d octets restants\n",paquets,n,remaining);
            	}
			}
            	if ((n = Rio_readn(clientfd, buffer, remaining))>0) {
					Rio_writen(fd, buffer, n);
					paquets++;
					remaining = remaining - n;
					printf("client : paquet (%d) reçu de taille (%d), %d octets restants\n",paquets,n,remaining);
				}
        }    
		printf("client : fin de la reception\n");
		printf("\n");
    }


}
/*end of the while*/


}
