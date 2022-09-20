#include <stdio.h>
#include <stdlib.h>
#include "server.h"

int main(int argc, char** argv) {
    printf("Arguments: %d\n", argc);
    if (argc > 0) {
        for (int i = 0; i < argc; ++i)
            printf("[0] = %s\n", argv[i]);
    }

    return 0;
}