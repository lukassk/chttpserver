#include "server.h"

chttp_server chttp_server_construct(int domain) {
    chttp_server server;
    
    server.domain = domain;

    return server;
}