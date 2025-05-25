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
#include <sys/sendfile.h>

#include "wrappers.h"
#include "helpers.h"




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
    else if (strcmp(extention, ".txt") == 0) return "text/plain; charset=utf-8";
    else if (strcmp(extention, ".css" ) == 0) return "text/css";
    else if (strcmp(extention, ".jpg") == 0 || strcmp(extention, ".jpeg") == 0) return "image/jpeg";
    else if (strcmp(extention, ".png") == 0) return "image/png";
    else if (strcmp(extention, ".pdf") == 0) return "application/pdf";
    else return "application/octet-stream";

}

int construct_header(char *buf, int code, char *code_interpr, char *content_type , ssize_t content_length){
    int written = snprintf(buf, RESPOSE_HEADER_LEN,
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %ld\r\n"
        "\r\n",
        code, code_interpr, content_type, content_length
    );
    return written;
}

void send_http_response_from_buf(int fd, int code, char *code_interpr, char *content_type , ssize_t content_length,char *body){
    char response_buf[RESPOSE_HEADER_LEN];
    int written = construct_header(response_buf, code, code_interpr, content_type, content_length);

    my_send(fd, response_buf, written, 0);
    my_send(fd, body, content_length, 0);
}

void send_http_response_from_file(int fd,int file_fd, struct stat st ,int code, char *code_interpr, char *content_type , ssize_t content_length){
    char response_buf[RESPOSE_HEADER_LEN];
    int written = construct_header(response_buf, code, code_interpr, content_type, content_length);

    my_send(fd, response_buf, written, 0);
    my_sendfile(fd, file_fd, NULL, st.st_size);
}

void send_redirect_response(int fd, const char *location) {
    char response_buf[RESPOSE_HEADER_LEN];
    int len = snprintf(response_buf, RESPOSE_HEADER_LEN,
        "HTTP/1.1 301 Moved Permanently\r\n"
        "Location: %s\r\n"
        "Content-Length: 0\r\n"
        "\r\n", location);

    my_send(fd, response_buf, len, 0);
}

static inline void send_error_response(int fd, int code, char *code_interpr, char *body){
    send_http_response_from_buf(fd, code , code_interpr, "text/html; charset=utf-8", strlen(body),body);
}

void send_template_error_response(int fd, int code, char *interp) {
    char buf[RESPONSE_ERROR_LEN];
    snprintf(buf, sizeof(buf), "<h1>%d %s</h1>", code, interp);
    send_error_response(fd, code, (char *)interp, buf);
}


int is_path_valid(int connection_fd, char *path, char *new_root){

    // Resolve path to file
    char resolved_path[MAX_PATH];
    if (!realpath(path, resolved_path)) {
        send_template_error_response(connection_fd, 404, "Not Found");
        return -1;
    }

    // Resolve path to current website root
    char resolved_root[MAX_PATH];
    if (!realpath(new_root, resolved_root)) {
        send_template_error_response(connection_fd, 500, "Internal Server Error");
        return -1;
    }

    // Check if prefixes match
    if (strncmp(resolved_path, resolved_root, strlen(resolved_root)) != 0) {
        send_template_error_response(connection_fd, 403, "Forbidden");
        return -1;
    }
    return 0;
}


// Runs main server logic
void run_server_logic(int connection_fd, char *root){
    char request_buffer[REQUEST_BUFFER_LEN];
    struct pollfd pfd = {.fd=connection_fd, .events=POLLIN};
    int keep_connection = 1;

    while (keep_connection) {
        int res = my_poll(&pfd, 1, TIMEOUT);
        if (res == 0) break;

        ssize_t len = recv(connection_fd, request_buffer, REQUEST_BUFFER_LEN - 1, 0);
        if (len <= 0) return;
        request_buffer[len] = '\0';

        // Scan request into below buffers
        char sub_path[256];
        char host[256];
        char connection[50];
        connection[0] = '\0';
        if(sscanf(request_buffer, "GET %255s HTTP/1.1\r\nHost: %255s\r\nConnection: %49s", sub_path, host, connection)<2){
            send_template_error_response(connection_fd, 501,"Not Implemented" );
            break;
        }
        
        // Check if connection should be closed after this request
        if(strcmp(connection, "close") == 0){
            keep_connection = 0;
        }

        // Removes port part from host
        char port[MAX_PORT] = "";
        char *colon = strchr(host, ':');
        if (colon) {
            *colon = '\0';
            snprintf(port, sizeof(port), ":%s", colon + 1);
        }

        DEBUG_PRINT("Subpath: %s, Host: %s\n", sub_path, host);

        char new_root[MAX_PATH];
        if(snprintf(new_root, sizeof(new_root),"%s/%s", root, host) >= MAX_PATH){
            send_template_error_response(connection_fd, 510,"Root path too long");
        }

        char path[MAX_PATH];
        if(snprintf(path, sizeof(path), "%s/%s%s", root, host, sub_path) >= MAX_PATH){
            send_template_error_response(connection_fd, 511,"Path too long");
        }

        
        //Check if path is inside a valid range
        if(is_path_valid(connection_fd, path, new_root) < 0){
            break;
        }

        struct stat file_stat;
        DEBUG_PRINT("Path: %s\n", path);
        if(stat(path, &file_stat) != 0){
            send_template_error_response(connection_fd, 404, "Not Found");
            break;
        }

        
        if(S_ISDIR(file_stat.st_mode)){
            char redir[MAX_PATH];
            if(snprintf(redir, sizeof(redir), "http://%s%s%sindex.html", host, port,sub_path) >= MAX_PATH){
                send_template_error_response(connection_fd, 512,"Redirection path too long");
            }
            send_redirect_response(connection_fd, redir);
            break;
        }

        int file_fd;
        if((file_fd = open(path, O_RDONLY)) < 0){
            send_template_error_response(connection_fd, 403, "Forbidden");
            break;
        }

        char *type = get_content_type(path);
        send_http_response_from_file(connection_fd,file_fd,file_stat, 200, "OK", type, file_stat.st_size);
        
        my_close(file_fd);
    }

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


    DEBUG_PRINT("Open server on port: %d with directory: %s\n", port, directory_path);


    // Create listening socket
    int server_fd = my_socket(AF_INET, SOCK_STREAM, 0);

    // Setup addres structure
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(struct sockaddr_in));
    setup_server_addr(&server_address, port);

    // Bind and setup listening socket
    my_bind(server_fd, (struct sockaddr*) &server_address, sizeof(server_address));
    my_listen(server_fd, BACKLOG);

    DEBUG_PRINT("Listening...\n");

    for(;;){
        int connect_fd = my_accept(server_fd, NULL, NULL);
        printf("Accepted\n");
        run_server_logic(connect_fd, directory_path);

        my_close(connect_fd);
    }

    my_close(server_fd);

}