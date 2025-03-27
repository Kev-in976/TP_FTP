/*
 * echoclient.c - An echo client
 */
 
#include "types.h"
#include "csapp.h"

int main(int argc, char **argv)
{
    int clientfd, port;
    char host[10];
    request_t req;
    response_t res;
    char buffer [1024];
    ssize_t n;

    if (argc != 2) {
        fprintf(stderr, "usage: <fichier> %s\n", argv[0]);
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
    printf("client connected to server OS\n"); 

	/*filling the request*/
    strcpy(req.filename, argv[1]);
    req.request = GET; /*on ne gère que ce type de requête so far*/
	
	/* sending the type of the request : GET | PUT | LS to the server */
	/*int bufreq=2;
	bufreq = req.request;
    Write(clientfd, &bufreq, sizeof(req.request));*/

    Rio_writen(clientfd, &(req.request), sizeof(req.request));
    Rio_writen(clientfd, &(req.filename), sizeof(req.filename));
	

	/* fetching the response from the server */
    Read(clientfd, &res.status, sizeof(res.status));

	/*status manage*/
    if (res.status == NOT_FOUND){
        printf("erreur 404, fichier introuvable");
    }
    else if (res.status == FOUND){
        int fd = Open("import.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd<0) {
            perror("Open error");
            exit(1);
        }
		
		printf("réception du fichier en cours ...\n");
        while((n = Rio_readn(clientfd, buffer, sizeof(buffer)))>0) {
            printf("%zd octets ont été lu\n",n);
            Rio_writen(fd, buffer, n);
        }
		printf("fin du transfert\n");
    }


    Close(clientfd);
    exit(0);
}
