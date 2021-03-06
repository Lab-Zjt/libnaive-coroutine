#ifndef SORANET_HOOK_H
#define SORANET_HOOK_H

#include <zconf.h>

struct sockaddr;

extern "C" {
typedef ssize_t (*read_t)(int fd, void *buf, size_t count);
typedef ssize_t (*write_t)(int fd, const void *buf, size_t count);
typedef int (*connect_t)(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
typedef int (*accept_t)(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
typedef ssize_t (*recv_t)(int sockfd, void *buf, size_t len, int flags);
typedef ssize_t (*send_t)(int sockfd, const void *buf, size_t len, int flags);
typedef int (*usleep_t)(useconds_t usec);
typedef unsigned int(*sleep_t)(unsigned int seconds);
typedef int (*nanosleep_t)(const struct timespec *req, struct timespec *rem);
extern read_t origin_read;
extern write_t origin_write;
extern connect_t origin_connect;
extern accept_t origin_accept;
extern recv_t origin_recv;
extern send_t origin_send;
extern usleep_t origin_usleep;
extern sleep_t origin_sleep;
extern nanosleep_t origin_nanosleep;
};

#endif
