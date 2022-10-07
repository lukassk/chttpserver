#include "header.h"

chttp_header*
chttp_header_construct(
    char *key,
    char *value
) {
    chttp_header *header = chttp_header_alloc();

    header->key = chttp_str_create(key);
    header->value = chttp_str_create(value);

    return header;
}

void
chttp_header_free(
    chttp_header* header
) {
    chttp_str_free(&header->key);
    chttp_str_free(&header->value);
    free(header);
}