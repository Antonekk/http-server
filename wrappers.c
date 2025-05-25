#include "wrappers.h"



int my_socket(int domain, int type, int protocol){
    int s_fd = socket(domain, type, protocol);
    if(!s_fd){
        error_exit("socket");
    }
    return s_fd;
}

int my_bind(int sockfd, struct sockaddr *addr, socklen_t addr_len)
{
    int result = bind(sockfd, addr, addr_len);
    if(result < 0){
        error_exit("bind");
    }
    return result;
}

int my_listen(int sockfd, int backlog)
{
    int result = listen(sockfd, backlog);
    if(result < 0){
        error_exit("listen");
    }
    return result;
}

int my_accept(int sockfd, struct sockaddr *addr, socklen_t *addr_len)
{
    int result = accept(sockfd, addr, addr_len);
    if(result < 0){
        error_exit("accept");
    }
    return result;
}

int my_poll(struct pollfd *fds, nfds_t nfds, int timeout){
    int res = poll(fds, nfds, timeout);
    if (res == -1) {
        perror("poll");
    }
    return res;
}



ssize_t my_send(int fd, char *buf, size_t n, int flags)
{
    size_t sent = 0;
    while(sent < n){
        ssize_t res = send(fd, buf + sent, n - sent, flags);
        if(res < 0){
            perror("send");
        }
        if(sent == 0) break;
        sent += res;
    }
    return sent;
}

ssize_t my_sendfile(int out_fd, int in_fd, off_t *offset, size_t count)
{
    size_t sent = 0;
    while(sent < count){
        ssize_t res = sendfile(out_fd, in_fd, offset, count);
        if(res < 0){
            perror("sendfile");
        }
        if(res == 0) break;
        sent += res;
    }
    
    return sent;
}

void *my_calloc(size_t nmemb, size_t size) {
    void *ptr = calloc(nmemb, size);
    if (!ptr)
      error_exit("Calloc error");
    return ptr;
}

int my_clock_gettime(clockid_t clockid, struct timespec *tp){
    int res = clock_gettime(clockid, tp);
    if(res < 0){
        error_exit("clock_gettime error");
    }
    return res;
}


int my_atoi(char *src){
    int result;
    if (!(result = atoi(src))){
        msg_exit("Can't convert port to int");
    }
    return result;
}

void my_stat(char *pathname, struct stat *statbuf)
{
    int res = stat(pathname, statbuf);
    if (res != 0){
        error_exit("stat");
    }
}


int my_close(int fd)
{
    int res = close(fd);
    if(res == -1){
        error_exit("close");
    }
    return res;
}

