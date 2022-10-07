#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "server.h"

#define DEBUG 1

chttp_server server;

void sig_handler(int sig)
{
    server.signal = sig;
}

void print_runtime_arguments(int argc, char** argv)
{
    printf("Arguments: %d\n", argc);
    if (argc > 0) {
        for (int i = 0; i < argc; ++i) {
            printf("[%d] = %s\n", i, argv[i]);
        }
    }
}

int main(int argc, char** argv)
{
    #ifdef DEBUG
        print_runtime_arguments(argc, argv);
    #endif

    signal(SIGINT, sig_handler);

    server = chttp_server_construct(AF_INET, SOCK_STREAM, 0, INADDR_ANY, 8000, 10);
    chttp_server_launch(&server);
    chttp_server_close(&server);

    return 0;
}