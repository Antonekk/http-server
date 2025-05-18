#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/socket.h>


#include "wrappers.h"
#include "helpers.h"

// Setup sockaddr_in structure for server 
void setup_server_addr(struct sockaddr_in *server_addr, int port_number, char *ip_addr){
    server_addr->sin_family = AF_INET;
    server_addr->sin_port = htons(port_number);

    // Translate string form of ip addr 
    my_inet_pton(AF_INET, ip_addr, &server_addr->sin_addr);

}


void run_server_logic(int connection_fd){
    return;
}

int main(int argc, char* argv[]){

    if(argc != 3){
        fprintf(stderr, "Usage: %s <port> <directory>\n", argv[0]);
    }

    int port = my_atoi(argv[1]);
    char *directory = argv[2];

    if(port < MIN_PORT || port > MAX_PORT){
        msg_exit("Port outside of valid range");
    }

    #ifdef DEBUG
    printf("Open server on port: %d with directory: %s\n", port, directory);
    #endif

    int server_fd = my_socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(struct sockaddr_in));
    setup_server_addr(&server_address, port, "127.0.0.1");

    my_bind(server_fd, (struct sockaddr*) &server_address, sizeof(server_address));

    my_listen(server_fd, BACKLOG);
    #ifdef DEBUG
    printf("Listening...\n");
    #endif

    for(;;){
        int connect_fd = my_accept(server_fd, NULL, NULL);

        struct timeval conn_time;
        conn_time.tv_usec = TIMEOUT * 1000;
        setsockopt(connect_fd, SOL_SOCKET, SO_RCVTIMEO, &conn_time, sizeof(conn_time));


        close(connect_fd);
    }







}