#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 1024

void error_handler(char *message) {
    perror(message);
    exit(1);//process termination
}

void receive_file(FILE *file, int socket, struct sockaddr_in client_address, socklen_t client_address_len) 
{
    char buffer[MAX_BUFFER_SIZE];
    ssize_t bytesRead; //Used for getting the count of bytes 

    while (1) //loop will run till the end of the file
    {
    /*ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,struct sockaddr *src_addr, socklen_t *addrlen);*/
        bytesRead = recvfrom(socket, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_address, &client_address_len);
        //storing the data received on the server socket to the buffer
        if (bytesRead == -1) {
           error_handler("recvfrom() failed");
        }

        if (bytesRead == 0) {
            break;  // End of file reached
        }

        fwrite(buffer, 1, bytesRead, file); // storing the data received on the buffer to the output file pointed by file pointer
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <output_filename> <listening_port>\n", argv[0]);
        exit(1);
    }

    char *output_filename = argv[1];//given by the user as a command line argument
    int listening_port = atoi(argv[2]);

    FILE *file = fopen(output_filename, "wb");
    if (!file) {
        error_handler("Unable to create output file");
    }

    // Create a UDP socket
    int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_socket == -1) {
        error_handler("socket() failed");
    }

    // Set up the server address structure
    struct sockaddr_in server_address, client_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    /*The htonl() function converts the unsigned integer hostlong from host byte order to network byte order */
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    /* INADDR_ANY :we can use it if don't  know the IP address of the machine it was running on */
    server_address.sin_port = htons(listening_port);

    // Bind the socket
    if (bind(udp_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        error_handler("bind() failed");
    }

    // Receive the file
    receive_file(file, udp_socket, client_address, sizeof(client_address));

    fclose(file);
    close(udp_socket);

    return 0;
}

