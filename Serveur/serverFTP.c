/*
 * echoserveri.c - An iterative echo server
 */


#include "csapp.h"

#define MAX_NAME_LEN 256

#define NPROC 3     //nombre limite d'enfants simultanné

int compteur = 0;   //nombre de fils en cours d'execution

pid_t lesFils [3];

void echo(int connfd);

typedef enum {
    GET,
    PUT,
    LS
} typereq_t;


typedef struct request__t{
    int requete;
    char nomFich [256];
}request_t;
//peut-etre que les types du dessus doivent uniquement être dans clientFTP

typedef struct response_t{
    int status;
}response_t;


/* 
 * Note that this code only works with IPv4 addresses
 * (IPv6 is not supported)
 */
void handler1(int sig) {
    pid_t pid;
    while((pid = waitpid(-1, NULL, WNOHANG)) > 0){
        printf("Handler reaped child %d\n", (int)pid);
    }

    return;
}

void handlerFils(int sig) {
    printf("fils %d terminé\n", getpid());
    exit(0);
}

void handlerPapa(int sig){

    for (int i = 0; i < NPROC; i++){
        Kill(lesFils[i],SIGINT);
        Waitpid(lesFils[i], NULL,0);
    }
    printf("tu as tué le papa\n");
    
    exit(0);
}

void traitementReq(int connfd){
    request_t req;
    response_t res;
    ssize_t n;
    int fd;
    char buffer[1024];
    ssize_t buff;

    if ( (n = Read(connfd, &req, sizeof(request_t))) > 0){  //On verifie qu'il n'y a pas d'erreur de lecture
        switch (req.requete){
            case GET:
                printf("Requête de type GET pour le fichier %s\n",req.nomFich);
                //fd = Open(req.nomFich, O_RDONLY, 0444);

                if ( (fd = Open(req.nomFich, O_RDONLY, 0444)) == -1){
                    perror("fichier introuvable");
                    res.status = 404;
                    Write(connfd, &res, sizeof(response_t));    //envoi de la reponse au client
                    return;
                }

                res.status = 200;
                printf("Envoi du status : %d\n", res.status);
                printf("Fichier %s trouvé.\n", req.nomFich);
                Write(connfd, &res, sizeof(response_t));    //envoi de la reponse au client

                /*copie du fichier à envoyer en memoire*/

                 while( (buff = Read(fd,buffer, sizeof(buffer))) > 0 ){     //on check l'etat de read, si >0 on continue
                    Write(connfd, buffer, sizeof(buffer));
                 }
                 close(fd);
                break;

            case PUT:
                printf("Requête de type PUT pour le fichier %s",req.nomFich);
                break;

            case LS:
                printf("Requête de type LS pour le fichier %s",req.nomFich);
                break;
            
            default:
                fprintf(stderr,"requete non reconnu\n");
                break;
        }
    }    
}

void child_work(int listenfd, socklen_t clientlen, struct sockaddr_in clientaddr) {
    int connfd;
    char client_ip_string[INET_ADDRSTRLEN];
    char client_hostname[MAX_NAME_LEN];

    clientlen = (socklen_t)sizeof(clientaddr);

    while (1) {
        // Le processus enfant attend une connexion
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);

        /* On récupère le nom du client */
        Getnameinfo((SA *)&clientaddr, clientlen,
                    client_hostname, MAX_NAME_LEN, 0, 0, 0);

        /* Récupère l'IP du client sous forme textuelle */
        Inet_ntop(AF_INET, &clientaddr.sin_addr, client_ip_string,
                  INET_ADDRSTRLEN);

        printf("server connected to %s (%s)\n", client_hostname,
               client_ip_string);

        // Traitement de la connexion et identification du type de la requete
        traitementReq(connfd);

        // La connexion est terminée, attendre une nouvelle connexion
        Close(connfd);
    }
}

int main(int argc, char **argv)
{
    int listenfd, port;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    /*if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }*/
    port = 2121;    //atoi(argv[1]);
    
    clientlen = (socklen_t)sizeof(clientaddr);

    listenfd = Open_listenfd(port);

    //Signal(SIGCHLD, handler1);


    for (int i = 0; i < NPROC; i++) {
        if ( (lesFils[i] = fork()) == 0) {
            // Processus enfant
            Signal(SIGINT, handlerFils);
            child_work(listenfd, clientlen, clientaddr);
            printf("ici");
            exit(0);
        }

    }

    Signal(SIGINT, handlerPapa);

    while (1) {
        pause(); 
    }
    exit(0);
}
