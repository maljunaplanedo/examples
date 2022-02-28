#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFFER_SIZE 65535

int has_connection = 0;
int running = 1;
int sock;

void turnoff()
{
    shutdown(sock, SHUT_RDWR);
    close(sock);
    exit(0);
}

void shutdown_handler(int signum)
{
    if (has_connection)
        running = 0;
    else
        turnoff();
}

void configure_signals()
{
    struct sigaction action = {.sa_handler = &shutdown_handler,
                               .sa_flags = SA_RESTART};
    sigaction(SIGTERM, &action, NULL);
    sigaction(SIGINT, &action, NULL);
}

void configure_socket(int port)
{
    struct sockaddr_in sockaddr = {
        .sin_family = AF_INET, .sin_port = htons(port), .sin_addr = 0};

    sock = socket(AF_INET, SOCK_STREAM, 0);
    bind(sock, (struct sockaddr*)&sockaddr, sizeof(struct sockaddr_in));

    listen(sock, SOMAXCONN);
}

int check_file(const char* path)
{
    if (access(path, F_OK) == -1) {
        return 404;
    } else if (access(path, R_OK) == -1) {
        return 403;
    } else if (access(path, X_OK) == 0) {
        return 1;
    }
    return 0;
}

ssize_t get_file(const char* path, char* dest)
{
    int fd = open(path, O_RDONLY);
    ssize_t size = read(fd, dest, BUFFER_SIZE);
    close(fd);
    return size;
}

void get_fullpath(char* dest, const char* directory, const char* filename)
{
    dest[0] = '\0';
    strcat(dest, directory);
    strcat(dest, "/");
    strcat(dest, filename);
}

void execute_file(const char* filename, int socket)
{
    if (fork() == 0) {
        dup2(socket, 1);
        execlp(filename, filename, NULL);
    } else {
        wait(NULL);
    }
}

int main(int argc, char** argv)
{
    configure_signals();
    configure_socket(atoi(argv[1]));

    const char* directory = argv[2];
    char buffer[BUFFER_SIZE];
    char file_buffer[BUFFER_SIZE];

    while (running) {
        int conn = accept(sock, NULL, NULL);

        has_connection = 1;
        size_t len = recv(conn, buffer, BUFFER_SIZE, 0);

        buffer[len] = '\0';
        strtok(buffer, " ");
        char* filename = strtok(NULL, " ");

        char fullpath[4096];
        get_fullpath(fullpath, directory, filename);

        ssize_t file_type = check_file(fullpath);
        buffer[0] = '\0';

        if (file_type == 1) {
            strcat(buffer, "HTTP/1.1 200 OK\r\n");
            send(conn, buffer, strlen(buffer), 0);
            execute_file(fullpath, conn);
        } else {
            if (file_type == 404) {
                strcat(buffer, "HTTP/1.1 404 Not Found\r\n\r\n");
            } else if (file_type == 403) {
                strcat(buffer, "HTTP/1.1 403 Forbidden\r\n\r\n");
            } else {
                strcat(buffer, "HTTP/1.1 200 OK\r\n");
                strcat(buffer, "Content-Length: ");
                sprintf(
                    buffer + strlen(buffer),
                    "%ld\r\n\r\n",
                    get_file(fullpath, file_buffer));
                strcat(buffer, file_buffer);
            }
            send(conn, buffer, strlen(buffer), 0);
        }

        shutdown(conn, SHUT_RDWR);
        close(conn);
        has_connection = 0;
    }

    turnoff();
}

