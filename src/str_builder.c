#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "str_builder.h"

chttp_str_node *
chttp_str_node_init(
    chttp_str *str
) {
    chttp_str_node *node = chttp_str_node_empty();
    node->str = chttp_str_create(str->content);

    return node;
}

void
_chttp_str_node_append_str(
    chttp_str_node **node,
    chttp_str *str
) {
    chttp_str_node *next;
    if (node != NULL && *node != NULL) {
        if ((*node)->str.length == 0) {
            (*node)->str = chttp_str_create(str->content);
            return;
        }
    }
    
    next = chttp_str_node_init(str);
    (*node)->next = next;
    *node = next;
}

void
chttp_str_node_append(
    chttp_str_node **node,
    char *ascii
) {
    chttp_str str = chttp_str_create(ascii);
    _chttp_str_node_append_str(node, &str);
}

void
chttp_str_node_appendf(
    chttp_str_node **node,
    const char *format,
    ...
) {
    char buffer[1024];

    va_list args;
    va_start(args, format);
    sprintf(buffer, format, args);
    vsnprintf(buffer, 1023, format, args);
    va_end(args);

    chttp_str str = chttp_str_create(buffer);
    _chttp_str_node_append_str(node, &str);
}