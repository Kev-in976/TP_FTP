typedef enum {
    GET,
    PUT,
    LS
} typereq_t;


typedef struct request_t{
    typereq_t request;
    char filename[256];
}request_t;

typedef enum {
	NOT_FOUND = 404,
	FOUND = 200,
    SENDING = 100
} status_t;

typedef struct response_t{
    status_t status;
}response_t;
