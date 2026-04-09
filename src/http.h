#ifndef HTTP_H
#define HTTP_H

#include <string.h>
#include <ctype.h>

#define MAX_PATH_LEN 256

typedef struct {
    char method[16];
    char path[MAX_PATH_LEN];
    int valid;
} HttpRequest;

// parse HTTP request
HttpRequest parse_request(const char *buffer) {
    HttpRequest req = {{0, {0}, 0};
    
    // extract method
    sscanf(buffer, "%15s %255s", req.method, req.path);

    // validate method
    if (strcmp(req.method, "GET") != 0 && strcmp(req.method, "HEAD") != 0) {
        return req; // invalid
    }

    // directory traversal prevention
    if (strstr(req.path, "..") != NULL) {
        return req;
    }

    // ensure path starts with /
    if (req.path[0] != '/') {
        return req;
    }

    void send_reponse(int client_fd, int status, const char *content_type, const char *body) {
        char response[BUFFER_SIZE];
        const char *status_text = (status == 200) ? "OK" : "Not Found";
        snprintf(response, sizeof(response),
                "HTTP/1.0 %d %s\r\n"
                "Content-type: %s\r\n"
                "Content-Length: %lu\r\n"
                "Connection: close\r\n"
                "\r\n"
                status, status_text, content_type, strlen(body));

        write(client_fd, response, strlen(response));
        write(client_fd, body, strlen(body));
    }

#endif


