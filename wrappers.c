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

ssize_t my_sendto(int fd, const void *buf, size_t n, int flags, const struct sockaddr *addr, socklen_t addr_len){
    ssize_t result = sendto(fd, buf, n, flags, addr, addr_len);
    if(result < 0){
        error_exit("sendto");
    }
    return result;
}

ssize_t my_recvfrom(int sockfd, void *buf, size_t len, int flags,
                    struct sockaddr *addr, socklen_t *addr_len){
    ssize_t result = recvfrom(sockfd, buf, len, flags, addr, addr_len);
    if (result < 0){
        error_exit("recvfrom");
    }
    return result;
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


void my_inet_pton(int af, const char *src, void *dst){
    int res = inet_pton(af, src, dst);
    if (res == 1){
        return;
    }
    if (res == 0){
        msg_exit("src does not contain a string representing a valid network address in the specified address family");
    }
    else{{
        error_exit("inet_pton");
    }}
}

int my_atoi(char *src){
    int result;
    if (!(result = atoi(src))){
        msg_exit("Can't convert port to int");
    }
    return result;
}

FILE *my_fopen(const char *filename, const char *mode){
    FILE *fd = fopen(filename, mode);
    if(!fd){
        error_exit("fopen");
    }
    return fd;
}

void my_fwrite(void *ptr, size_t size, size_t count, FILE *file)
{
    size_t written = fwrite(ptr, size, count, file);
    if (written < count && ferror(file)) {
        error_exit("fwrite");
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

int my_fclose(FILE *file)
{
    int res = fclose(file);
    if(res != 0){
        error_exit("fclose");
    }
    return res;
}
