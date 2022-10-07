#ifndef STR_H
#define STR_H

struct chttp_str {
    char *content;
    unsigned int length;
};

typedef struct chttp_str chttp_str;

chttp_str
chttp_str_create(
    char *ptr
);

chttp_str
chttp_str_sub(
    char *ptr,
    int start,
    int length
);

chttp_str
chttp_str_from_decimal(
    int decimal
);

void
chttp_str_free(
    chttp_str *s
);

#endif