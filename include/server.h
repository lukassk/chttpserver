#ifndef SERVER_H
#define SERVER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "request.h"
#include "response.h"
#include "protocol.h"

/* CHTTP package server structure
    responsible for the network socket initialization and configuration */
struct chttp_server {
    int domain;
    int service;
    int protocol;
    u_long interface;
    int port;
    int backlog;

    struct sockaddr_in address;

    int socket;
};

typedef struct chttp_server chttp_server;

/* Constructs an HTTP server structure and attaches a request handler to
    the listener */
chttp_server
chttp_server_construct(
    int domain,
    int service,
    int protocol,
    u_long interface,
    int port,
    int backlog
);

/* Launches the CHTTP server */
void
chttp_server_launch(
    chttp_server* server
);

/* Closes the CHTTP server */
void
chttp_server_close(
    chttp_server* server
);

/* Parses request coming from a socket, constructs a CHTTP request
    structure and returns it */
chttp_request
chttp_server_parse_request(
    int fd
);

/* Executes through the CHTTP server handlers serving the request details
    and returns a response */
chttp_response
chttp_server_handle_request(
    chttp_request req
);

/* Formats and writes CHTTP response to the socket */
int
chttp_server_write_response(
    int socket,
    chttp_response res
);

#endif // SERVER_H