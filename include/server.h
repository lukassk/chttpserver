#ifndef SERVER_H
#define SERVER_H

typedef
struct chttpserver {
    int domain;
} chttpserver;

chttpserver create_chttpserver(int);

#endif // SERVER_H