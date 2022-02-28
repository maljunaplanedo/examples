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
#include <sys/epoll.h>
#include <sys/signalfd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFFER_SIZE 4096

int configure_sigterm(int epoll)
{
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGTERM);

    sigprocmask(SIG_BLOCK, &sigset, NULL);
    int fd = signalfd(-1, &sigset, 0);

    struct epoll_event sigterm_event = {.events = EPOLLIN, .data.fd = fd};

    epoll_ctl(epoll, EPOLL_CTL_ADD, fd, &sigterm_event);

    return fd;
}

int start_server(int epoll, int port)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in sockaddr = {
        .sin_addr = 0, .sin_port = htons(port), .sin_family = AF_INET};

    bind(sock, (struct sockaddr*)&sockaddr, sizeof(struct sockaddr_in));

    struct epoll_event new_client_event = {.events = EPOLLIN, .data.fd = sock};

    epoll_ctl(epoll, EPOLL_CTL_ADD, sock, &new_client_event);
    listen(sock, SOMAXCONN);

    return sock;
}

void register_client(int epoll, int sock)
{
    int conn = accept(sock, NULL, NULL);

    struct epoll_event client_event = {.events = EPOLLIN, .data.fd = conn};

    epoll_ctl(epoll, EPOLL_CTL_ADD, conn, &client_event);
}

void handle_client(int client)
{
    char buffer[BUFFER_SIZE];

    ssize_t bytes = read(client, buffer, BUFFER_SIZE);
    for (size_t i = 0; i < bytes; ++i) {
        if ('a' <= buffer[i] && buffer[i] <= 'z') {
            buffer[i] = buffer[i] - 'a' + 'A';
        }
    }
    write(client, buffer, bytes);
}

int main(int argc, char** argv)
{
    uint16_t port = atoi(argv[1]);

    int epoll = epoll_create1(0);

    int sigterm_fd = configure_sigterm(epoll);
    int server_fd = start_server(epoll, port);

    for (;;) {
        struct epoll_event event;
        epoll_wait(epoll, &event, 1, -1);

        int fd = event.data.fd;
        if (fd == sigterm_fd) {
            exit(0);
        } else if (fd == server_fd) {
            register_client(epoll, server_fd);
        } else {
            handle_client(fd);
        }
    }
}

