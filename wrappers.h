#ifndef WRAPPERS_H
#define WRAPPERS_H

#include <arpa/inet.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <poll.h>
#include <unistd.h>
#include "helpers.h"

void my_inet_pton(int af, const char *src, void *dst);

int my_socket(int domain, int type, int protocol);

int my_bind(int sockfd, struct sockaddr *addr, socklen_t addr_len);
int my_listen(int sockfd, int backlog);
int my_accept(int sockfd, struct sockaddr *addr, socklen_t *addr_len);

int my_poll(struct pollfd *fds, nfds_t nfds, int timeout);

ssize_t my_sendto(int fd, const void *buf, size_t n, int flags, 
                    const struct sockaddr *addr, socklen_t addr_len);

ssize_t my_recvfrom(int sockfd, void *buf, size_t len, int flags,
                    struct sockaddr *addr, socklen_t *addr_len);

void *my_calloc(size_t nmemb, size_t size);

int my_clock_gettime(clockid_t clockid, struct timespec *tp);

int my_atoi(char *src);

FILE *my_fopen(const char *filename, const char *mode);
void my_fwrite(void *ptr, size_t size, size_t count, FILE *file);

int my_close(int fd);
int my_fclose(FILE *file);



#endif