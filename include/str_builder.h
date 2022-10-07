#ifndef STR_BUILDER_H
#define STR_BUILDER_H

#include "str.h"

struct chttp_str_node {
    struct chttp_str str;
    struct chttp_str_node *next;
};

typedef struct chttp_str_node chttp_str_node;

chttp_str_node *
chttp_str_node_init(
    chttp_str *str
);

#define chttp_str_node_empty() \
        ((chttp_str_node *) calloc(1, sizeof(chttp_str_node)))

void
_chttp_str_node_append_str(
    chttp_str_node **node,
    chttp_str *str
);

void
chttp_str_node_append(
    chttp_str_node **node,
    char *ascii
);

void
chttp_str_node_appendf(
    chttp_str_node **node,
    const char *format,
    ...
);

#endif