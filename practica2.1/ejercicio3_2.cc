#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

int main(int argc, char **argv){
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags = AI_PASSIVE; // Devolver 0.0.0.0
    hints.ai_family = AF_UNSPEC; // Coge direcciones de cualquier familia (ipv4, ipv6...)
    hints.ai_socktype = SOCK_DGRAM;

    struct addrinfo *result;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &result);
    if(rc != 0) {
        std::cerr << "Error getaddrinfo " << gai_strerror(rc) << std::endl;
        return -1;
    }

    int sd = socket(result->ai_family, result->ai_socktype, 0);

    bind(sd, (struct sockaddr *) result->ai_addr, result->ai_addrlen);

    bool exit = false;
    while(!exit){
        struct sockaddr_in server;
        server.sin_family = result->ai_family;
        server.sin_addr = *((struct in_addr *)result->ai_addr->sa_data);
        server.sin_port = htons((int)argv[2]);
        socklen_t server_len = result->ai_addrlen;
        
        sendto(sd, argv[3], 1, 0, (struct sockaddr *) &server, server_len);
    }

    return 0;
}