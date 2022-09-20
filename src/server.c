#include "server.h"

chttpserver create_chttpserver(int domain) {
    chttpserver server;
    
    server.domain = domain;

    return server;
}