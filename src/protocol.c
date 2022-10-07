#include "protocol.h"

char *
chttp_protocol2ascii(
    enum chttp_protocols protocol
) {
    switch (protocol) {
        /* TODO: implement later, no point uncommenting now until backing
            functionality is there */
        // case CHTTP_PROTOCOL_HTTP2_0:
        //     return "HTTP/2.0";

        default:
            return "HTTP/1.1";
    }
}