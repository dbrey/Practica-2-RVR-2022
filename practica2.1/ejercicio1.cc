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

    while(1){
        int buffersize = 80;
        char buffer[buffersize];
        struct sockaddr_in client;
        socklen_t client_len = sizeof(struct sockaddr_in);
        int bytes = recvfrom(sd, buffer, buffersize, 0, (struct sockaddr*) &client, &client_len);
        buffer[bytes] = '\0';

        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];
        getnameinfo((struct sockaddr *) &client, client_len, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

        printf("Conxión desde Host: %s  Puerto: %s\n", host, serv);
        printf("\tMensaje(%i): %s\n", bytes, buffer);

        sendto(sd, buffer, bytes, 0, (struct sockaddr *) &client, client_len);
    }

    return 0;
}