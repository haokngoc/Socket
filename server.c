#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#define PORT 5000
#define CHUNK_SIZE 4096

int main() {
    int server_socket, client_socket, err_check;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_len = sizeof(client_address);
    char filename[1024];
    long int filesize;
    FILE *file;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating server socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(PORT);

    err_check = bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    if (err_check == -1) {
        perror("Error binding socket to port");
        exit(EXIT_FAILURE);
    }

    err_check = listen(server_socket, 5);
    if (err_check == -1) {
        perror("Error listening for incoming connections");
        exit(EXIT_FAILURE);
    }

    client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len);
    if (client_socket == -1) {
        perror("Error accepting client connection");
        exit(EXIT_FAILURE);
    }

    err_check = recv(client_socket, filename, 1024, 0);
    if (err_check == -1) {
        perror("Error receiving filename and filesize");
        exit(EXIT_FAILURE);
    }

    filename[err_check] = '\0';
    char *token = strtok(filename, "_");
    
    filesize = atol(strtok(NULL, "_"));
    file = fopen(token, "wb");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char buffer[CHUNK_SIZE];
    long int total_bytes_read = 0;
    while (total_bytes_read < filesize) {
        int bytes_read = recv(client_socket, buffer, CHUNK_SIZE, 0);
        if (bytes_read == -1) {
            perror("Error receiving file data");
            exit(EXIT_FAILURE);
        }

        int bytes_written = fwrite(buffer, sizeof(char), bytes_read, file);
        if (bytes_written < bytes_read) {
            perror("Error writing file");
            exit(EXIT_FAILURE);
        }

        total_bytes_read += bytes_read;
    }

    fclose(file);
    close(client_socket);
    close(server_socket);

    printf("File received successfully\n");

    return 0;
}
