#include "response.h"

char *
chttp_response2ascii(
    enum chttp_responses response
) {
    switch (response) {
        case CHTTP_RESPONSE_OK:
            return "OK";
        case CHTTP_RESPONSE_FORBIDDEN:
            return "Forbidden";
        case CHTTP_RESPONSE_NOT_FOUND:
            return "Not Found";
        case CHTTP_RESPONSE_INTERNAL_SERVER_ERROR:
            return "Internal Server Error";
        case CHTTP_RESPONSE_NOT_IMPLEMENTED:
            return "Not Implemented";
        case CHTTP_RESPONSE_SERVICE_UNAVAILABLE:
            return "Service Unavailable";
        default:
            return "Unassigned";
    }
}