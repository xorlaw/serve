#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#define MAX_FILE_SIZE 1048576

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 4096
#endif

static const char *mime_type(const char *path) {
    const char *ext = strrchr(path, '.');
    if (!ext)
        return "text/plain";
    if (strcmp(ext, ".html") == 0 || strcmp(ext, ".htm") == 0)
        return "text/html";
    return "text/plain";
}

static int write_all(int fd, const char *buf, size_t len) {
    while (len > 0) {
        ssize_t w = write(fd, buf, len);
        if (w < 0)
            return -1;
        buf += w;
        len -= (size_t)w;
    }
    return 0;
}

int serve_file(int client_fd, const char *filepath) {
    if (strstr(filepath, "..") != NULL)
        return -1;

    FILE *file = fopen(filepath, "rb");
    if (!file)
        return -1;

    struct stat st;
    if (fstat(fileno(file), &st) < 0 || st.st_size > MAX_FILE_SIZE) {
        fclose(file);
        return -1;
    }

    char response[BUFFER_SIZE];
    int n = snprintf(response, sizeof(response),
             "HTTP/1.0 200 OK\r\n"
             "Content-Type: %s\r\n"
             "Content-Length: %ld\r\n"
             "Connection: close\r\n"
             "\r\n",
             mime_type(filepath), (long)st.st_size);

    if (n < 0 || (size_t)n >= sizeof(response)) {
        fclose(file);
        return -1;
    }

    if (write_all(client_fd, response, (size_t)n) < 0) {
        fclose(file);
        return -1;
    }

    char buf[4096];
    size_t bytes;
    while ((bytes = fread(buf, 1, sizeof(buf), file)) > 0) {
        if (write_all(client_fd, buf, bytes) < 0)
            break;
    }

    fclose(file);
    return 0;
}

#endif
