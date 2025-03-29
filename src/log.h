#include <stdbool.h>

typedef struct log_t {
	int request;
	char filename[256];
	int filesize;
	int paquets_recus;
} log_t;

bool existslog();

log_t readlog();

