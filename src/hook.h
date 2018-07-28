#ifndef SORANET_HOOK_H
#define SORANET_HOOK_H

#include <zconf.h>

struct sockaddr;

extern "C" {
typedef ssize_t (*read_t)(int fd, void *buf, size_t count);
typedef ssize_t (*write_t)(int fd, const void *buf, size_t count);
typedef int (*close_t)(int fd);
typedef int (*accept_t)(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
typedef int (*printf_t)(const char *__restrict format, ...);
extern read_t origin_read;
extern write_t origin_write;
extern close_t origin_close;
extern accept_t origin_accept;
extern printf_t origin_printf;
void hook_all() __attribute__((constructor(101)));
};

#endif
