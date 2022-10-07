#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "server.h"
#include "macros.h"
#include "str_builder.h"

chttp_server
chttp_server_construct(
    int domain,
    int service,
    int protocol,
    u_long interface,
    int port,
    int backlog
) {
    chttp_server server;
    
    server.domain = domain;
    server.service = service;
    server.protocol = protocol;
    server.interface = interface;
    server.port = port;
    server.backlog = backlog;

    server.address.sin_family = domain;
    server.address.sin_port = htons(port);
    server.address.sin_addr.s_addr = htonl(interface);

    server.socket = socket(domain, service, protocol);

    if (server.socket == 0) {
        perror("Failed to initialize socket\n");
        exit(1);
    }

    if (
        bind(
            server.socket,
            (struct sockaddr *)&server.address,
            sizeof(server.address)
        ) < 0
    ) {
        perror("Failed to bind socket...\n");
        exit(2);
    };

    if (listen(server.socket, server.backlog) < 0) {
        perror("Failed to start listening to socket");
        exit(3);
    };

    return server;
}

void
chttp_server_launch(
    chttp_server* server
) {
    int address_length = sizeof(server->address);
    int new_socket;

    chttp_request req;
    chttp_response res;

    if (server) {
        // TODO: eradicate the endless loop, implement signaling from somewhere
        while (1) {
            new_socket = accept(
                server->socket,
                (struct sockaddr *)&server->address,
                (socklen_t *)&address_length
            );

            req = chttp_server_parse_request(new_socket);
            res = chttp_server_handle_request(req);
            chttp_server_write_response(new_socket, res);

            // Right now we don't handle keep-alive connections
            // although we cannot close the connection immediately cause some
            // packets might still be in transit and we don't want to send an
            // RST packet before the last one arrives
            usleep(100);
            close(new_socket);
        }
    }
}

void
chttp_server_close(
    chttp_server* server
) {
    if (server)
        close(server->socket);
}

chttp_request
chttp_server_parse_request(
    int fd
) {
    char buffer[CHTTP_REQUEST_BUFFER_SIZE];
    char a, b, c; // temporary characters
    int i, length; // temporary integers
    char* buffer_pos = buffer;

    // int flag_incorrect_method = 0,
    //     flag_incorrect_path = 0,
    //     flag_incorrect_protocol = 0,
    int flag_dont_continue = 0;

    enum chttp_request_methods method = CHTTP_REQUEST_METHOD_UNDEFINED;

    // parsing determining the http request method from socket
    // doing it this way will allow the sPonGebOB casing in the method,
    // not that it's useful but in a well optimised form for that
    read(fd, buffer, CHTTP_REQUEST_BUFFER_SIZE);
    a = buffer[0];
    b = buffer[1];
    c = buffer[2];
    if (
        (a == 'g' || a == 'G') &&
        (b == 'e' || b == 'E') &&
        (c == 't' || c == 'T')
    ) {
        a = buffer[3];
        if (a == ' ') {
            method = CHTTP_REQUEST_METHOD_GET;
            buffer_pos += 4;
        }
    }
    else if (a == 'p' || a == 'P') {
        // P
        if (
            (b == 'o' || b == 'O') &&
            (c == 's' || c == 'S')
        ) {
            // POS
            a = buffer[3];
            if (a == 't' || a == 'T') {
                a = buffer[4];
                if (a == ' ') {
                    method = CHTTP_REQUEST_METHOD_POST;
                    buffer_pos += 5;
                }
            }
        }
        else if (
            (b == 'u' || b == 'U') &&
            (c == 't' || c == 'T')
        ) {
            // PUT
            a = buffer[3];
            if (a == ' ') {
                method = CHTTP_REQUEST_METHOD_PUT;
                buffer_pos += 4;
            }
        }

    }
    else if (a == 'd' || a == 'D') {
        // D
        if (
            (b == 'e' || b == 'E') &&
            (c == 'l' || c == 'L')
        ) {
            // DEL
            a = buffer[3];
            b = buffer[4];
            c = buffer[5];
            if (
                (a == 'e' || a == 'E') &&
                (b == 't' || b == 'T') &&
                (c == 'e' || c == 'E')
            ) {
                a = buffer[6];
                if (a == ' ') {
                    method = CHTTP_REQUEST_METHOD_DELETE;
                    buffer_pos += 7;
                }
            }
        }
    } else {
        // flag_incorrect_method = 1;
        flag_dont_continue = 1;
    }

    // if at this point we have a defined method we're all fine, syntax correct
    chttp_request req;
    req = chttp_request_construct(method);

    if (flag_dont_continue) {
        return req;
    }

    // cut the path and move pointer forward
    i = length = 0;
    while (1) {
        a = *(buffer_pos + i);
        if (a == ' ') {
            if (i > 0) {
                length = min(i, CHTTP_REQUEST_PATH_SIZE - 1);
                strncpy(req.path, buffer_pos, length);
                req.path[length] = 0;
            }
            ++length;
            buffer_pos += length;
            break;
        } else if (a == 0 || a == '\n' || a == '\r') {
            // shit is on fire
            // flag_incorrect_path = 1;
            flag_dont_continue = 1;
            break;
        }
        ++i;
    }

    if (flag_dont_continue) {
        return req;
    }

    while (*buffer_pos == '\n' || *buffer_pos == '\r') {
        ++buffer_pos;
    }

    i = length = 0;
    while (1) {
        a = *(buffer_pos + i);
        if (a == '\n' || a == '\r') {
            if (i > 0) {
                length = min(i, CHTTP_REQUEST_PROTOCOL_SIZE - 1);
                strncpy(
                    req.protocol,
                    buffer,
                    length
                );
                req.protocol[length] = 0;
            }
            buffer_pos += length;
            break;
        } else if (a == 0) {
            // flag_incorrect_protocol = 1;
            flag_dont_continue = 1;
            break;
        }
        ++i;
    }

    char line[256];

    while (buffer_pos[0] == '\n' || buffer_pos[0] == '\r') {
        ++buffer_pos;
    }

    chttp_header *new_header;
    char *colon;
    int colon_pos;
    while (sscanf(buffer_pos, "%255[^\n\r]s", line) != -1) {
        new_header = chttp_header_alloc();
        colon_pos = -1;
        length = strlen(line);
        colon = strstr(line, ":");
        if (colon != NULL) {
            colon_pos = colon - line;
        }

        if (colon_pos != -1) {
            new_header->key = chttp_str_sub(line, 0, colon_pos);
            new_header->value = chttp_str_sub(
                line,
                colon_pos + 2,
                length - colon_pos - 1
            );
        } else {
            chttp_header_free(new_header);
            new_header = NULL;
        }

        buffer_pos += length;
        while (*buffer_pos == '\n' || *buffer_pos == '\r') {
            ++buffer_pos;
        }

        if (new_header) {
            printf(
                "%s: %s\n",
                new_header->key.content,
                new_header->value.content
            );
        }
    }

    return req;
}

chttp_response
chttp_server_handle_request(
    chttp_request req
) {
    chttp_response res;

    res.response_code = CHTTP_RESPONSE_OK;
    res.headers = NULL;
    if (req.method == CHTTP_REQUEST_METHOD_UNDEFINED) {
        res.body[0] = 0;
        res.response_code = CHTTP_RESPONSE_INTERNAL_SERVER_ERROR;
    }
    else {
        sprintf(
            res.body,
            "Hello, World!\n\n<pre>You requested for:\n%s</pre>",
            req.path
        );
    }

    return res;
}

void
_chttp_server_write_pointer(
    int socket,
    char *content
) {
    int length = strlen(content);
    if (length > 0) {
        write(socket, content, length);
    }
}

void
_chttp_server_write_str_node(
    int socket,
    chttp_str_node *head
) {
    chttp_str_node *node;
    int total_length = 0;

    if (head == NULL) {
        return;
    }

    node = head;
    while (node != NULL) {
        total_length += node->str.length;
        node = node->next;
    }

    char *buffer;
    int i = 0;
    buffer = (char *)calloc(total_length, sizeof(char));
    node = head;
    while (node != NULL) {
        strncpy(buffer + i, node->str.content, node->str.length);
        i += node->str.length;
        node = node->next;
    }

    printf("Response:\n%s\n", buffer);
    write(socket, buffer, total_length);
    free(buffer);
}

int
chttp_server_write_response(
    int socket,
    chttp_response res
) {
    chttp_str_node *head = chttp_str_node_empty();
    chttp_str_node *builder;

    builder = head;
    // append protocol and response code
    chttp_str_node_appendf(
        &builder,
        "HTTP/1.1 %d %s\n",
        res.response_code,
        chttp_response2ascii(res.response_code)
    );
    // append headers
    chttp_str_node_append(&builder, "Host: localhost:8000\n");
    chttp_str_node_append(&builder, "Content-Type: text/html\n");
    chttp_str_node_appendf(&builder, "Content-Length: %d\n", strlen(res.body));
    chttp_str_node_append(&builder, "\n");
    chttp_str_node_append(&builder, res.body);

    _chttp_server_write_str_node(socket, head);

    return 0;
}