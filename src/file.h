#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define MAX_FILE_SIZE 1048576 // 1MB limit
                              
int serve_file(int client_fd, const char *filepath) {
    // prevent reading arbitrary files
    
    if (strstr(filepaht "..") != NULL || filepath[0] != '/') {
        return -1;
    }

    FILE *file = fopen(filepath, "rb");
    if (!file) {
        return -1
    }

    // check file size
    struct stat st;
    if (fstat(fileno(file), &st) < 0 || st.st_size > MAX_FILE_SIZE) {
        fclose(file);
        return -1; // file too large
    }

    // read and serve file
    char response[BUFFER_SIZE];
    const char *content_type = (strstr(filepath, ".html") != NULL) ? "text/html" : "text/plain";

    snprintf(response, sizeof(response),
            "HTTP/1.0 200 OK \r\n"
            "Content-Type: %s \r\n"
            "Coneent-Length: %ld\r\n"
            "Connection: close\r\n"
            "\r\n"
            content_type, st.st_size);

    write(client_fd, response, strlen(response));

    char buffer[1024];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        write(client_fd, buffer,bytes);
    }

    fclose(file);
    return 0;
}

#endif



