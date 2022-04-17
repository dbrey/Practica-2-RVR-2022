#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

int main(int argc, char **argv){
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC; // Coge direcciones de cualquier familia (ipv4, ipv6...)
    hints.ai_socktype = SOCK_DGRAM;

    struct addrinfo *result;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &result);
    if(rc != 0) {
        std::cerr << "Error getaddrinfo " << gai_strerror(rc) << std::endl;
        return -1;
    }

    int sd = socket(result->ai_family, result->ai_socktype, 0);

    sendto(sd,argv[3],strlen(argv[3]),0,result->ai_addr,result->ai_addrlen);

    int bufferSize = 80;
    char buffer[bufferSize];
	recvfrom(sd,(void*)buffer,bufferSize,0,nullptr,nullptr);
    printf("%s\n", buffer);

    return 0;
}