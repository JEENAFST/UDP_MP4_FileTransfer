#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 1024
FILE *fp; //File pointer declaraction
void error_handler(char *message) {
    perror(message);
    exit(1);
}

void send_file(FILE *file, int socket, struct sockaddr_in server_address, socklen_t server_address_len) 
   {
    char buffer[MAX_BUFFER_SIZE];
    size_t bytesRead;
 //size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
 //reading 1 byte of data at a time from the file and storing it into character buffer 
    while ((bytesRead = fread(buffer,1, sizeof(buffer),fp)) > 0) //fp is the file pointer
       {
 /*ssize_t sendto(intsockfd, const void *buf, size_t len,int flags,const struct sockaddr *dest_addr, socklen_taddrlen);*/
 /*The sendto() function shall send a message through a connection-mode or connectionless-mode socket. If the socket is connectionless-mode, the message shall be sent to the address specified by dest_addr. If the socket is connection-mode, dest_addr shall be ignored. */
        if (sendto(socket, buffer, bytesRead, 0, (struct sockaddr *)&server_address, server_address_len) == -1) 
        {
         error_handler("sendto() failed");
        }
      /* while loop continuously read the data from the file and store it in a temporary file, from the buffer it is sending the data to the destination server till the EOF */
       }

    // Send an empty packet to signal the end of the file
    if (sendto(socket, NULL, 0, 0, (struct sockaddr *)&server_address, server_address_len) == -1) {
       error_handler("sendto() failed");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) 
    {
    
        fprintf(stderr, "Usage: %s <filename> <server_ip> <server_port>\n", argv[0]);
        exit(1);
    }

    char *filename = argv[1];
    char *server_ip = argv[2];
    int server_port = atoi(argv[3]);// port number converted into integer

    fp = fopen(filename, "rb"); //Open for reading in binary mode, filename will be given as a comment line argument(Eg. video.mp4)
    if (!fp) {
        error_handler("File not found");
    }

    // Create a UDP socket
    //int socket(int domain, int type, int protocol); protocol=0(Default one)
    int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);// SOCK_DGRAM used for UDP socket
    if (udp_socket == -1) {
        error_handler("socket() failed");
    }

    // Set up the server address structure
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address)); //clearing the memory space of server_address variable
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(server_ip);/* inter_addr :function converts the Internet host address server_ip from IPv4 numbers-and-dots notation into binary data in network byte order */
    //server_ip can be given as 127.0.0.1 for the local host (Given as command line argument)
    server_address.sin_port = htons(server_port);/*The htons() function converts the unsigned short integer hostshort from host byte order to network byte order */

    // Send the file
    send_file(fp, udp_socket, server_address, sizeof(server_address)); //calling the defined function
    /* function will transfer the file pointed by file pointer fp to the server_address through UDP socket */

    fclose(fp); //closing the file
    close(udp_socket); //closing the UDP socket

    return 0;
}

