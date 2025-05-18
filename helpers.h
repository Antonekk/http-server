// Antoni Strasz 339096

#ifndef HELPERS_H
#define HELPERS_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DEBUG


#define TIMEOUT 100

#define MAX_TO_RECIVE 10000000

#define MIN_PORT 1024
#define MAX_PORT 65535

#define MAX_REQUEST_LEN 150

#define MAX_RESPONSE_LEN 1200
#define MAX_RESPONSE_BODY_LEN 1000


// Error handling
void error_exit(const char *msg);
void msg_exit(const char *msg);



#endif