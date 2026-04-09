#include "server.h"
#include "http.h"
#include "file.h"
#include <signal.h>
void handle_client(int client_fd) {
    char buffer[BUFFER_SIZE] = {0};
    ssize_t bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes <= 0) {
        close(client_fd);
        return;
    }
    buffer[bytes] = '\0';
    HttpRequest req = parse_request(buffer);
    if (!req.valid) {
        send_response(client_fd, 400, "text/plain", "400 Bad Request\n");
        close(client_fd);
        return;
    }
    // default to index.html if just "/"
    char filepath[MAX_PATH_LEN + 6];
    snprintf(filepath, sizeof(filepath), ".%s%s",
            req.path,
            (strcmp(req.path, "/") == 0) ? "index.html" : "");
    if (serve_file(client_fd, filepath) < 0) {
        send_response(client_fd, 404, "text/plain", "404 Not Found\n");
    }
    close(client_fd);
}
int main(void) {
    signal(SIGPIPE, SIG_IGN); // ignore broken pipe
    int server_fd = create_server_socket();
    printf("serving files from current directory (ctrl + c to quit)\n");
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    while (1) {
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("accept failed");
            continue;
        }
        handle_client(client_fd);
    }
    close(server_fd);
    return 0;
}


