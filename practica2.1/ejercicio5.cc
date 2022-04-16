#include <netdb.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

const size_t MESSAGE_MAX_SIZE = 100;

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cerr << "Parámetros incorrectos\n Formato: .\ejercicio_5 <direccion> <puerto> \n ";
        return -1;
    }

    struct addrinfo hints;
    struct addrinfo *result;

    memset((void *)&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;       // Coge direcciones de cualquier familia (ipv4, ipv6...)
    hints.ai_socktype = SOCK_STREAM; //TCP

    int rc = getaddrinfo(argv[1], argv[2], &hints, &result);
     if(rc != 0) {
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

    // Nos conectamos al servidor
    int sd_servidor = connect(sd, (struct sockaddr *)result->ai_addr, result->ai_addrlen);

    if (sd_servidor == -1)
    {
        std::cerr << "No ha sido posible conectarse al servidor [connect]\n"<< gai_strerror(sd_servidor) << std::endl;
        return -1;
    }

    freeaddrinfo(res); // Eliminamos  memoria innecesaria

    bool exit = false;
    char buffer[MESSAGE_MAX_SIZE];

    while (!exit)
    {
        std::cin >> buffer;

        //Enviamos informacion al servidor
        ssize_t bytesSend = send(sd_servidor, buffer, (MESSAGE_MAX_SIZE - 1)*sizeof(char), 0);

        // Finalizamos la conexion
        if (buffer[0] == 'Q' && buffer[1] == '\0')
        {
            exit = true;
            continue;
        }

        //Recibimos respuesta del servidor
        recv(sd_servidor, buffer,(MESSAGE_MAX_SIZE - 1)*sizeof(char),0);

        std::cout << buffer << '\n';
    }

    // Cerramos todo
    close(sd_servidor);
    close(sd);

    return 0;