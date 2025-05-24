// Antoni Strasz 339096

#ifndef HELPERS_H
#define HELPERS_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DEBUG

#ifdef DEBUG
#define DEBUG_PRINT(...) fprintf(stderr, __VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif

#define TIMEOUT 500


#define REQUEST_BUFFER_LEN 1024
#define RESPONSE_ERROR_LEN 1024

#define RESPOSE_HEADER_LEN 2048

#define MAX_PATH 256

#define MIN_PORT 1024
#define MAX_PORT 65535
#define MAX_PORT_LEN 16

#define BACKLOG 64


// Error handling
void error_exit(const char *msg);
void msg_exit(const char *msg);



#endif