#ifndef SORANET_HOOK_H
#define SORANET_HOOK_H

#include <zconf.h>
#include <malloc.h>

struct sockaddr;

extern "C" {
typedef ssize_t (*read_t)(int fd, void *buf, size_t count);
typedef ssize_t (*write_t)(int fd, const void *buf, size_t count);
typedef int (*open_t)(const char *pathname, int flags, ...);
typedef int (*close_t)(int fd);
typedef int (*socket_t)(int domain, int type, int protocol);
typedef int (*connect_t)(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
typedef int (*bind_t)(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
typedef int (*listen_t)(int sockfd, int backlog);
typedef int (*accept_t)(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
typedef int (*printf_t)(const char *format, ...);
typedef void *(*malloc_t)(size_t size);
extern read_t origin_read;
extern write_t origin_write;
extern open_t origin_open;
extern close_t origin_close;
extern socket_t origin_socket;
extern connect_t origin_connect;
extern bind_t origin_bind;
extern listen_t origin_listen;
extern accept_t origin_accept;
extern printf_t origin_printf;
extern malloc_t origin_malloc;
void hook_all() __attribute__((constructor(110)));
void *__wrap_malloc(size_t size);
void __wrap_free(void *ptr);
};

#endif
