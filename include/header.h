#ifndef HEADER_H
#define HEADER_H

#include <stdlib.h>
#include "str.h"

#define CHTTP_HEADER_KEY_LENGTH 64
#define CHTTP_HEADER_VALUE_LENGTH 256

/* Structure to hold http header identifier and value */
struct chttp_header {
    chttp_str key;
    chttp_str value;
};

typedef struct chttp_header chttp_header;

#define chttp_header_alloc() \
        ((chttp_header *)calloc(1, sizeof(chttp_header)))

/* Constructs a header structure provided two pointers to two character arrays
    respectively */
chttp_header*
chttp_header_construct(
    char *key,
    char *value
);

#endif