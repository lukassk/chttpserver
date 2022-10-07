#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "server.h"

#define DEBUG 1

int last_sig = 0;

void sig_handler(int sig)
{
    last_sig = sig;

    // exit is not a good solution, might interrupt some process
    exit(1);
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

    chttp_server s = chttp_server_construct(AF_INET, SOCK_STREAM, 0, INADDR_ANY, 8000, 10);

    chttp_server_launch(&s);
    
    chttp_server_close(&s);

    return 0;
}