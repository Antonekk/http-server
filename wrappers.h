#ifndef WRAPPERS_H
#define WRAPPERS_H

#include <arpa/inet.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <poll.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include "helpers.h"


int my_socket(int domain, int type, int protocol);

int my_bind(int sockfd, struct sockaddr *addr, socklen_t addr_len);
int my_listen(int sockfd, int backlog);
int my_accept(int sockfd, struct sockaddr *addr, socklen_t *addr_len);

int my_poll(struct pollfd *fds, nfds_t nfds, int timeout);


ssize_t my_send(int fd, char *buf, size_t n, int flags);
ssize_t my_sendfile(int out_fd, int in_fd, off_t *offset, size_t count);

void *my_calloc(size_t nmemb, size_t size);

int my_clock_gettime(clockid_t clockid, struct timespec *tp);

int my_atoi(char *src);

void my_stat(char *pathname, struct stat *statbuf);


int my_close(int fd);




#endif