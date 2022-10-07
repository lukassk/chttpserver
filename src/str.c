#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "str.h"

/* Do not forget that this uses calloc under the hood and you should release
    your memory using chttp_str_free(&chttp_str) */
chttp_str
chttp_str_create(
    char *ptr
) {
    chttp_str s;

    if (ptr != NULL) {
        s.length = strlen(ptr);
        if (s.length > 0) {
            s.content = (char *)calloc(s.length + 1, sizeof(char));
            strncpy(s.content, ptr, s.length);
            s.content[s.length] = 0;
        }
    }

    return s;
}

/* Do not forget that this uses calloc under the hood and you should release
    your memory using chttp_str_free(&chttp_str) */
chttp_str
chttp_str_sub(
    char *ptr,
    int start,
    int length
) {
    chttp_str s;

    if (ptr != NULL) {
        s.length = length;
        if (s.length > 0) {
            s.content = (char *)calloc(length + 1, sizeof(char));
            strncpy(s.content, ptr + start, length);
            s.content[length] = 0;
        }
    }

    return s;
}

chttp_str
chttp_str_from_decimal(
    int decimal
) {
    // TODO: fix this sometime
    char buffer[256];
    sprintf(buffer, "%d", decimal);

    return chttp_str_create(buffer);
}

void
chttp_str_free(
    chttp_str *s
) {
    if (s != NULL) {
        free(s->content);
        s->length = 0;
    }
}