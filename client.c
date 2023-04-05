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

void main() {
    int client_socket, err_check;
    struct sockaddr_in server_address;
    char filename[1024];
    long int filesize;
    FILE *file;

    // Prompt user for file path
    // printf("Enter file path: ");
    // scanf("%s", filepath);

    char filepath[] = "friends-final.txt";
    file = fopen(filepath, "rb");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0L, SEEK_END);
    filesize = ftell(file);
    fseek(file, 0L, SEEK_SET);

    char *filename_start = strrchr(filepath, '/');
    if (filename_start == NULL) {
        filename_start = filepath;
    } else {
        filename_start++;
    }
    strncpy(filename, filename_start, sizeof(filename));
    char filesize_str[20];
    sprintf(filesize_str, "_%ld", filesize);
    strncat(filename, filesize_str, sizeof(filename) - strlen(filename) - 1);

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Error creating client socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port = htons(PORT);

    err_check = connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    if (err_check == -1) {
        perror("Error connecting to server");
        exit(EXIT_FAILURE);
    }

    err_check = send(client_socket, filename, strlen(filename), 0);
    if (err_check == -1) {
        perror("Error sending filename and filesize");
        exit(EXIT_FAILURE);
    }

    char buffer[CHUNK_SIZE];
    long int total_bytes_sent = 0;
    while (total_bytes_sent < filesize) {
        int bytes_read = fread(buffer, sizeof(char), CHUNK_SIZE, file);
        if (bytes_read == 0) {
            perror("Error reading file");
            exit(EXIT_FAILURE);
        }

        int bytes_sent = send(client_socket, buffer, bytes_read, 0);
        if (bytes_sent == -1) {
            perror("Error sending file data");
            exit(EXIT_FAILURE);
        }

        total_bytes_sent += bytes_sent;
    }

    fclose(file);
    close(client_socket);

    printf("File sent successfully\n");
}
///home/hao/Documents/cl/friends-final.txt
