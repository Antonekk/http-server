// Antoni Strasz 339096

#ifndef HELPERS_H
#define HELPERS_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DEBUG


#define TIMEOUT 1000


#define MIN_PORT 1024
#define MAX_PORT 65535

#define BACKLOG 64


// Error handling
void error_exit(const char *msg);
void msg_exit(const char *msg);



#endif