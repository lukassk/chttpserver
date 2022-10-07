#ifndef REQUEST_H
#define REQUEST_H

#include "header.h"

#define CHTTP_REQUEST_BUFFER_SIZE 65535
#define CHTTP_REQUEST_PATH_SIZE 256
#define CHTTP_REQUEST_PROTOCOL_SIZE 16

enum chttp_request_methods {
    CHTTP_REQUEST_METHOD_UNDEFINED = 0,
    CHTTP_REQUEST_METHOD_GET = 1,
    CHTTP_REQUEST_METHOD_POST = 2,
    CHTTP_REQUEST_METHOD_PUT = 3,
    CHTTP_REQUEST_METHOD_DELETE = 4,
};



struct chttp_request {
    enum chttp_request_methods method;
    char path[CHTTP_REQUEST_PATH_SIZE];
    char protocol[CHTTP_REQUEST_PROTOCOL_SIZE];

    chttp_header *headers;
    int headers_count;

    char *body;
};

typedef struct chttp_request chttp_request;

chttp_request
chttp_request_construct(
    enum chttp_request_methods method
);

/* Converts the numeric value of request method identifier to the pointer value
    of a character array representing the method in ASCII characters */
const char *chttp_request_method2string(
    enum chttp_request_methods method
);

#endif