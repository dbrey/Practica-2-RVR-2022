#include <netdb.h>
#include <iostream>
#include <string.h>
#include <unistd.h>

//Sockets
#include <sys/types.h>
#include <sys/socket.h>

const size_t MESSAGE_MAX_SIZE = 100;

int main(int argc, char **argv)
{
    
    if (argc != 3)
    {
        std::cerr << "Parámetros incorrectos\n Formato: .\ejercicio_4 <direccion> <puerto>\n ";
        return -1;
    }

    struct addrinfo hints;
    struct addrinfo *result;

    memset((void *)&hints, 0, sizeof(struct addrinfo));

    hints.ai_flags = AI_PASSIVE;     //Devolver 0.0.0.0
    hints.ai_family = AF_UNSPEC; // Coge direcciones de cualquier familia (ipv4, ipv6...)
    hints.ai_socktype = SOCK_STREAM; //TCP

    int rc = getaddrinfo(argv[1], argv[2], &hints, &result);

    // Fallo desde RC
    if (rc != 0)
    {
        std::cerr << "Error getaddrinfo " << gai_strerror(rc) << std::endl;
        return -1;
    }

    int sd = socket(result->ai_family, result->ai_socktype, 0);

    // No se creo el socket
    if (sd == -1)
    {
        std::cerr << "Fallo en la creacion del [socket]\n" << gai_strerror(sd) << std::endl;
        return -1;
    }

    // Metemos el addr en el socket
    bind(sd, result->ai_addr, result->ai_addrlen)

    // El servidor escucha en el socket
    // 1 unica conexion a la vez
    if(listen(sd, 1) == -1)
    {
        std::cerr << "Fallo en el [listen]\n" << gai_strerror(sd) << std::endl;
        return -1;
    }

    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    struct sockaddr client;
    socklen_t clientLength = sizeof(struct sockaddr);
    int cliente_sd = accept(sd, (struct sockaddr *)&client, &clientLength);

    if(cliente_sd == -1)
    {
        std::cerr << "Fallo en la conexion TCP con el cliente [accept]\n" << gai_strerror(sd) << std::endl;
    } 

    getnameinfo((struct sockaddr *)&client, clientLength, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
    printf("%d bytes de %s:%s\n", bytes, host, serv);


    //Echo
    bool exit = true;
    while (exit)
    {
        char buffer[MESSAGE_MAX_SIZE];

        // Recibimos del cliente
        ssize_t bytesRecieved = recv(cliente_sd, (void *) buffer, (MESSAGE_MAX_SIZE -1 )* sizeof(char), 0);

        if( bytesRecieved <= 0 )
        {
            std::cout << "Conexion finalizada\n";
            exit = false;
            continue;
        }
        
        // Reenviamos la info del server al cliente
        send(cliente_sd, (void *) buffer, bytesRecieved, 0);
    }

    // Cerramos todo
    close(cliente_sd);
    close(sd);

    return 0;
}