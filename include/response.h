#ifndef RESPONSE_H
#define RESPONSE_H

#define CHTTP_RESPONSE_BUFFER_SIZE 65535

enum chttp_responses {
    CHTTP_RESPONSE_OK = 200,
    CHTTP_RESPONSE_FORBIDDEN = 403,
    CHTTP_RESPONSE_NOT_FOUND = 404,
    CHTTP_RESPONSE_INTERNAL_SERVER_ERROR = 500,
    CHTTP_RESPONSE_NOT_IMPLEMENTED = 501,
    CHTTP_RESPONSE_SERVICE_UNAVAILABLE = 503
};

struct chttp_response {
    int response_code;
    char *headers;
    char body[CHTTP_RESPONSE_BUFFER_SIZE];
};

typedef struct chttp_response chttp_response;

char *
chttp_response2ascii(
    enum chttp_responses response
);

#endif