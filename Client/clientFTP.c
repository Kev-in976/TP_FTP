/*
 * echoclient.c - An echo client
 */
#include "csapp.h"

typedef enum {
    GET,
    PUT,
    LS
} typereq_t;


typedef struct request__t{
    int requete;
    char nomFich [256];
}request_t;

typedef struct response_t{
    int status;
}response_t;


int main(int argc, char **argv)
{
    int clientfd, port;
    char *host;
    request_t req;
    response_t res;
    char buffer [1024];
    ssize_t n;

    if (argc != 3) {
        fprintf(stderr, "usage: %s <host> <nomFich>\n", argv[0]);
        exit(0);
    }
    host = argv[1];
    port = 2121; //atoi(argv[2]);

    /*
     * Note that the 'host' can be a name or an IP address.
     * If necessary, Open_clientfd will perform the name resolution
     * to obtain the IP address.
     */
    clientfd = Open_clientfd(host, port);
    
    /*
     * At this stage, the connection is established between the client
     * and the server OS ... but it is possible that the server application
     * has not yet called "Accept" for this connection
     */
    printf("client connected to server OS\n"); 
    
    req.requete = GET;
    strcpy(req.nomFich, argv[2]);
    Write(clientfd, &req, sizeof(request_t));
    printf("le code status avant read est %d\n",res.status);
    Read(clientfd, &res, sizeof(response_t));
    printf("le code status est %d\n",res.status);

    if (res.status == 404){
        printf("404 error, Fichier introuvable");
    }
    else if (res.status == 200){
        int fd = Open("fichier.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
            perror("Open");
            exit(1);
        }

        while( (n = Read(clientfd, buffer, sizeof(buffer))) > 0) {
            printf("%zd octet ont été lu\n",n);
            Write(fd, buffer, n);
        }
    }



    Close(clientfd);
    exit(0);
}
