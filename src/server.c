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
            sleep(1);
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
    char a, b, c; // temporary characters for comparisons
    int i; // iterators

    // int flag_incorrect_method = 0,
    //     flag_incorrect_path = 0,
    //     flag_incorrect_protocol = 0,
        int flag_dont_continue = 0;

    enum chttp_request_methods method = CHTTP_REQUEST_METHOD_UNDEFINED;

    // parsing determining the http request method from socket
    // doing it this way will allow the sPonGebOB casing in the method,
    // not that it's useful but in a well optimised form for that
    read(fd, buffer, 3);
    a = buffer[0];
    b = buffer[1];
    c = buffer[2];
    if (
        (a == 'g' || a == 'G') &&
        (b == 'e' || b == 'E') &&
        (c == 't' || c == 'T')
    ) {
        read(fd, buffer, 1);
        a = buffer[0];
        if (a == ' ') {
            method = CHTTP_REQUEST_METHOD_GET;
        }
    }
    else if (a == 'p' || a == 'P') {
        // P
        if (
            (b == 'o' || b == 'O') &&
            (c == 's' || c == 'S')
        ) {
            // POS
            read(fd, buffer, 1);
            a = buffer[0];
            if (a == 't' || a == 'T') {
                read(fd, buffer, 1);
                a = buffer[0];
                if (a == ' ') {
                    method = CHTTP_REQUEST_METHOD_POST;
                }
            }
        }
        else if (
            (b == 'u' || b == 'U') &&
            (c == 't' || c == 'T')
        ) {
            // PUT
            read(fd, buffer, 1);
            a = buffer[0];
            if (a == ' ') {
                method = CHTTP_REQUEST_METHOD_PUT;
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
            // all is fine but we need more bytes to confirm
            read(fd, buffer, 3);
            a = buffer[0];
            b = buffer[1];
            c = buffer[2];
            if (
                (a == 'e' || a == 'E') &&
                (b == 't' || b == 'T') &&
                (c == 'e' || c == 'E')
            ) {
                read(fd, buffer, 1);
                a = buffer[0];
                if (a == ' ') {
                    method = CHTTP_REQUEST_METHOD_DELETE;
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

    i = 0;
    while (1) {
        read(fd, buffer + i, 1);
        a = buffer[i];
        if (a == ' ') {
            if (i > 0) {
                strncpy(req.path, buffer, min(i, CHTTP_REQUEST_PATH_SIZE - 1));
                req.path[min(i, CHTTP_REQUEST_PATH_SIZE - 1)] = 0;
            }
            break;
        } else if (a == 0 || a == '\n') {
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
    
    buffer[i + 1] = 0;
    a = buffer[0];
    if (a != '/') {
        // flag_incorrect_path = 1;
        flag_dont_continue = 1;
    }

    if (flag_dont_continue) {
        return req;
    }

    i = 0;
    while (1) {
        read(fd, buffer + i, 1);
        a = buffer[i];
        if (a == '\n' || a == '\r') {
            if (i > 0) {
                strncpy(
                    req.protocol,
                    buffer,
                    min(i, CHTTP_REQUEST_PROTOCOL_SIZE - 1)
                );
                req.protocol[min(i, CHTTP_REQUEST_PROTOCOL_SIZE - 1)] = 0;
            }
            break;
        } else if (a == 0) {
            // flag_incorrect_protocol = 1;
            flag_dont_continue = 1;
            break;
        }
        ++i;
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