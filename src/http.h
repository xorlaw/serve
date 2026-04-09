#ifndef HTTP_H
#define HTTP_H

#include <stdio.h>
#include <string.h>
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

    if (sscanf(buffer, "%15s %255s", req.method, req.path) != 2)
        return req;

    if (strcmp(req.method, "GET") != 0 && strcmp(req.method, "HEAD") != 0)
        return req;

    if (strstr(req.path, "..") != NULL)
        return req;

    if (req.path[0] != '/')
        return req;

    req.valid = 1;
    return req;
}

static const char *status_text(int status) {
    switch (status) {
        case 200: return "OK";
        case 400: return "Bad Request";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 500: return "Internal Server Error";
        default:  return "Unknown";
    }
}

void send_response(int client_fd, int status, const char *content_type, const char *body) {
    char response[BUFFER_SIZE];
    size_t body_len = strlen(body);

    int n = snprintf(response, sizeof(response),
             "HTTP/1.0 %d %s\r\n"
             "Content-Type: %s\r\n"
             "Content-Length: %zu\r\n"
             "Connection: close\r\n"
             "\r\n",
             status, status_text(status), content_type, body_len);

    if (n < 0 || (size_t)n >= sizeof(response))
        return;

    write(client_fd, response, (size_t)n);
    write(client_fd, body, body_len);
}

#endif
