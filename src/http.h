#ifndef HTTP_H
#define HTTP_H

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_PATH_LEN 256

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 4096
#endif

typedef struct {
    char method[16];
    char path[MAX_PATH_LEN];
    int valid;
} HttpRequest;

HttpRequest parse_request(const char *buffer) {
    HttpRequest req = {{0}, {0}, 0};

    sscanf(buffer, "%15s %255s", req.method, req.path);

    if (strcmp(req.method, "GET") != 0 && strcmp(req.method, "HEAD") != 0)
        return req;

    if (strstr(req.path, "..") != NULL)
        return req;

    if (req.path[0] != '/')
        return req;

    req.valid = 1;
    return req;
}

void send_response(int client_fd, int status, const char *content_type, const char *body) {
    char response[BUFFER_SIZE];
    const char *status_text = (status == 200) ? "OK" :
                              (status == 400) ? "Bad Request" : "Not Found";
    snprintf(response, sizeof(response),
             "HTTP/1.0 %d %s\r\n"
             "Content-Type: %s\r\n"
             "Content-Length: %zu\r\n"
             "Connection: close\r\n"
             "\r\n",
             status, status_text, content_type, strlen(body));
    write(client_fd, response, strlen(response));
    write(client_fd, body, strlen(body));
}

#endif
