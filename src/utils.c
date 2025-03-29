#include <stdlib.h>
#include <stdio.h>

#ifndef UTILS_C
#define UTILS_C
#include "utils.h"
#endif
char * servpath(char * filename) {
	char *path = malloc(sizeof(char)*256);
	snprintf(path, 256, "./Server/%s", filename);	
	return path;
}

char * clipath(char * filename) {
	char *path = malloc(sizeof(char)*256);
	snprintf(path, 256, "./Client/%s", filename);	
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
}

