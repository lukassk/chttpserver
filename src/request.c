#include "request.h"

chttp_request
chttp_request_construct(
    enum chttp_request_methods method
) {
    chttp_request req;

    req.method = method;

    return req;
}

const char *chttp_request_method2string(
    enum chttp_request_methods method
) {
    switch (method) {
        case CHTTP_REQUEST_METHOD_GET:
            return "GET";
        case CHTTP_REQUEST_METHOD_POST:
            return "POST";
        case CHTTP_REQUEST_METHOD_PUT:
            return "PUT";
        case CHTTP_REQUEST_METHOD_DELETE:
            return "DELETE";
        default:
            return "UNDEFINED";
    }
}