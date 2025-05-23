#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <poll.h>

#include "wrappers.h"
#include "helpers.h"


#define REQUEST_BUFFER_LEN 1024
#define RESPONSE_BUFFER_LEN 100000


// Setup sockaddr_in structure for server 
void setup_server_addr(struct sockaddr_in *server_addr, int port_number){
    server_addr->sin_family = AF_INET;
    server_addr->sin_port = htons(port_number);
    server_addr->sin_addr.s_addr = INADDR_ANY;

}


char *get_content_type(char *ext){
    if (strcmp(ext, ".html") == 0) return "text/html; charset=utf-8";
    else if (strcmp(ext, ".txt") == 0) return "text/plain, charset=utf-8";
    else if (strcmp(ext, ".css" ) == 0) return "text/css";
    else if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) return "image/jpeg";
    else if (strcmp(ext, ".png")) return "image/png";
    else if (strcmp(ext, ".pdf")) return "application/pdf";
    else return "application/octet-stream";

}

void send_http_response(int fd, int code, char *code_interpr, char *content_type , char *body){
    char response_buf[RESPONSE_BUFFER_LEN];
    int written = snprintf(response_buf, RESPONSE_BUFFER_LEN,
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %ld\r\n"
        "\r\n"
        "%s",
        code, code_interpr, content_type, strlen(body),body
    );

    send(fd, response_buf, written, 0);
}

static inline void send_error_response(int fd, int code, char *code_interpr, char *body){
    send_http_response(fd, code , code_interpr, "text/html; charset=utf-8", body);
}


// Runs main server logic
void run_server_logic(int connection_fd, char *root){
    char request_buffer[REQUEST_BUFFER_LEN];
    struct pollfd pfd = {.fd=connection_fd, .events=POLLIN};

    int res = my_poll(&pfd, 1, TIMEOUT);
    if (res == 0) return;

    ssize_t len = recv(connection_fd, request_buffer, REQUEST_BUFFER_LEN - 1, 0);
    if (len <= 0) return;
    request_buffer[len] = '\0';

    if(strncmp(request_buffer, "GET ", 4) != 0){
        send_error_response(connection_fd, 501, "Not Implemented","<h1> Not Implemented </h1>");
    }

    send_error_response(connection_fd, 501, "Not Implemented","<h1> Not Implemented </h1>");

    return;
}

int main(int argc, char* argv[]){

    // Check for proper usage
    if(argc != 3){
        fprintf(stderr, "Usage: %s <port> <directory>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Assign program arguments
    int port = my_atoi(argv[1]);
    char *directory_path = argv[2];

    // Check if given port is contained in valid range
    if(port < MIN_PORT || port > MAX_PORT){
        msg_exit("Port outside of valid range");
    }

    struct stat server_dir;
    my_stat(directory_path, &server_dir);

    if(!S_ISDIR(server_dir.st_mode)){
        msg_exit("Invalid directory");
    }

    #ifdef DEBUG
    printf("Open server on port: %d with directory: %s\n", port, directory_path);
    #endif

    // Create listening socket
    int server_fd = my_socket(AF_INET, SOCK_STREAM, 0);

    // Setup addres structure
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(struct sockaddr_in));
    setup_server_addr(&server_address, port);

    // Bind and setup listening socket
    my_bind(server_fd, (struct sockaddr*) &server_address, sizeof(server_address));
    my_listen(server_fd, BACKLOG);

    #ifdef DEBUG
    printf("Listening...\n");
    #endif

    for(;;){
        int connect_fd = my_accept(server_fd, NULL, NULL);
        printf("Accepted\n");

        /*
        struct timeval conn_time;
        conn_time.tv_usec = TIMEOUT * 1000;
        setsockopt(connect_fd, SOL_SOCKET, SO_RCVTIMEO, &conn_time, sizeof(conn_time));
        */
        run_server_logic(connect_fd, directory_path);

        close(connect_fd);
    }

    close(server_fd);







}