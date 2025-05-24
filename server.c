#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <poll.h>
#include <fcntl.h>

#include "wrappers.h"
#include "helpers.h"


#define REQUEST_BUFFER_LEN 1024
#define RESPONSE_BUFFER_LEN 1000000


// Setup sockaddr_in structure for server 
void setup_server_addr(struct sockaddr_in *server_addr, int port_number){
    server_addr->sin_family = AF_INET;
    server_addr->sin_port = htons(port_number);
    server_addr->sin_addr.s_addr = INADDR_ANY;

}


char *get_content_type(char *file_path){
    char *extention = strrchr(file_path, '.');
    if(!extention) return "application/octet-stream";
    if (strcmp(extention, ".html") == 0) return "text/html; charset=utf-8";
    else if (strcmp(extention, ".txt") == 0) return "text/plain, charset=utf-8";
    else if (strcmp(extention, ".css" ) == 0) return "text/css";
    else if (strcmp(extention, ".jpg") == 0 || strcmp(extention, ".jpeg") == 0) return "image/jpeg";
    else if (strcmp(extention, ".png") == 0) return "image/png";
    else if (strcmp(extention, ".pdf") == 0) return "application/pdf";
    else return "application/octet-stream";

}

void send_http_response(int fd, int code, char *code_interpr, char *content_type , ssize_t content_length,char *body){
    char response_buf[RESPONSE_BUFFER_LEN];
    int written = snprintf(response_buf, RESPONSE_BUFFER_LEN,
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %ld\r\n"
        "\r\n",
        code, code_interpr, content_type, content_length
    );

    send(fd, response_buf, written, 0);
    send(fd, body, content_length, 0);
}

static inline void send_error_response(int fd, int code, char *code_interpr, char *body){
    send_http_response(fd, code , code_interpr, "text/html; charset=utf-8", strlen(body),body);
}

static inline void send_not_implemented_response(int fd){
    send_error_response(fd, 501, "Not Implemented","<h1> Not Implemented </h1>");
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

    char sub_path[256];
    char host[256];
    if(sscanf(request_buffer, "GET %256s HTTP/1.1\r\nHost: %256s", sub_path, host)!=2){
        send_not_implemented_response(connection_fd);
        return;
    }

    // Removes port part from host
    char *colon = strchr(host, ':');
    if (colon) {
        *colon = '\0';
    }

    #ifdef DEBUG
    printf("Subpath: %s, Host: %s\n", sub_path, host);
    #endif
    
    char path[256];
    snprintf(path, sizeof(path), "%s/%s%s", root, host, sub_path);
    char new_root[256];
    snprintf(new_root, sizeof(new_root),"%s/%s", root, host);

    // Check if after path resolution, resolved path has propper prefix
    // TODO, 
    /*
    char resolved_path[256];
    if (!realpath(path, resolved_path) || strncmp(resolved_path, new_root, strlen(new_root)) != 0) {
        printf("\n\nRoot: %s\nNew Root: %s\n Path: %s\nResolved: %s\n", root, new_root, path,resolved_path);
        send_error_response(connection_fd, 403, "Forbidden", "<h1>Forbidden</h1>");
        return;
    }
    */

    struct stat file_stat;
    printf("Path: %s\n", path);
    if(stat(path, &file_stat) != 0){
        send_error_response(connection_fd, 404, "Not Found", "<h1> Not Found </h1>");
    }

    int file_fd;
    if((file_fd = open(path, O_RDONLY)) < 0){
        send_error_response(connection_fd, 403, "Forbidden", "<h1>Forbidden</h1>");
    }

    char *type = get_content_type(path);

    char *response_buffer = calloc(RESPONSE_BUFFER_LEN, sizeof(char));
    ssize_t response_len = 0;

    ssize_t bytes_read;
    while ((bytes_read = read(file_fd, 
                            response_buffer + response_len, 
                            RESPONSE_BUFFER_LEN - response_len)) > 0) {
        response_len += bytes_read;
    }
    send_http_response(connection_fd, 200, "OK", type, response_len,response_buffer);
    close(file_fd);
    free(response_buffer);

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