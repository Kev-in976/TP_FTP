#ifndef LOG_H
#define LOG_H
#include "log.h"
#endif

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
bool existslog() {
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
