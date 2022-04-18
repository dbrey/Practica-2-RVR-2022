#include <netdb.h>
#include <iostream>
#include <string.h>
#include <unistd.h>

//Sockets
#include <sys/types.h>
#include <sys/socket.h>

//Threads
#include <thread>

const size_t MESSAGE_MAX_SIZE = 100;
#define MAX_LISTEN 5


//Clase para gestionar los threads
class MessageThread
{
private:
    int sd;

public:
    MessageThread(int cliente) : sd(cliente) {}
    void conexion()
    {
        bool serverActive = true;
        while (serverActive)
        {
            char buffer[MESSAGE_MAX_SIZE];
            ssize_t bytesRecieved = recv(sd, (void *)buffer, (MESSAGE_MAX_SIZE - 1) * sizeof(char), 0);
            if (bytesRecieved <= 0)
            {
                std::cout << "Conexion terminada\n";
                serverActive = false;
                continue;
            }
            send(sd, (void *)buffer, bytesRecieved, 0);
        }
        close(sd);
    }
};

int main(int argc, char **argv)
{
    struct addrinfo hints;
    struct addrinfo *result;

    memset((void *)&hints, 0, sizeof(struct addrinfo));

    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &result);

    if (rc != 0)
    {
        std::cerr << "Error getaddrinfo " << gai_strerror(rc) << std::endl;
        return -1;
    }

    int sd = socket(result->ai_family, result->ai_socktype, 0);

    bind(sd, result->ai_addr, result->ai_addrlen);
    listen(sd, MAX_LISTEN);

    int id = 0;

    while (true)
    {
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        struct sockaddr client;
        socklen_t clientLength = sizeof(struct sockaddr);
        int client_sd = accept(sd, (struct sockaddr *)&client, &clientLength);

        //Gestion de errores
        if (client_sd == -1)
        {
            std::cerr << "No se ha aceptado la conexion TCP con el cliente [accept]\n";
        }

        getnameinfo((struct sockaddr *)&client, clientLength, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
        std:: cout << "Conexión desde Host: "<< host <<" Puerto: " << serv<< "\n";

        MessageThread *m = new MessageThread(client_sd);
        std::thread([&m]() {m->conexion(); delete m; }).detach();
    }
    close(sd);

    return 0;
}