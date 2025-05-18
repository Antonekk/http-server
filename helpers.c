#include "helpers.h"


void error_exit(const char *msg){
     fprintf(stderr, "%s: %s\n", msg, strerror(errno));
     exit(EXIT_FAILURE);
}

void msg_exit(const char *msg){
    fprintf(stderr, "Exit: %s\n", msg);
    exit(EXIT_FAILURE);
}