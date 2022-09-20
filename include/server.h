#ifndef SERVER_H
#define SERVER_H

typedef struct {
    int domain;
} chttp_server;

chttp_server chttp_server_construct(int);

#endif // SERVER_H