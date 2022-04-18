#include <netdb.h>
#include <iostream>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <thread>

const size_t MESSAGE_MAX_SIZE = 100;

class MessageThread
{
private:
    int sd;
    int id;

public:
    MessageThread(int sd_, int i) : sd(sd_), id(i) {}

    void Crear_mensaje()
    {
        time_t rTime;
        size_t tam;
        char buffer[MESSAGE_MAX_SIZE];
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        struct sockaddr client;
        socklen_t clientLength = sizeof(struct sockaddr);

        while (true)
        {
            sleep(3);

            int bytes = recvfrom(sd, (void *)buffer, (MESSAGE_MAX_SIZE - 1) * sizeof(char), 0, &client, &clientLength);
            buffer[MESSAGE_MAX_SIZE] = '\0';         

            if (bytes == -1)
            {
                std::cerr << "Error en la recepcion de bytes, cerrando conexión\n";
                return;
            }

            getnameinfo(&client, clientLength, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
            std::cout << bytes << " bytes de " << host << ":" << serv << ". Thread: " << std::this_thread::get_id() << "\n";

            time(&rTime);
            switch (buffer[0])
            {
            case 't': // Hora
                
                tam = strftime(buffer, MESSAGE_MAX_SIZE - 1, "%T %p", localtime(&rTime));
                sendto(sd, buffer, tam, 0, &client, clientLength);
                break;

            case 'd': // Fecha
                tam = strftime(buffer, MESSAGE_MAX_SIZE - 1, "%F", localtime(&rTime));
                sendto(sd, buffer, tam, 0, &client, clientLength);
                break;

            default: // Otro desconocido
                std::cout << "Comando no soportado: " << buffer[0] << "\n";
                sendto(sd, "Comando no valido\n", 19, 0, &client, clientLength);
            }
        }
    }
};

int main(int argc, char **argv)
{

    struct addrinfo hints;
    struct addrinfo *result;

    memset((void *)&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;      // Coge direcciones de cualquier familia (ipv4, ipv6...)
    hints.ai_socktype = SOCK_DGRAM; //UDP

    int rc = getaddrinfo(argv[1], argv[2], &hints, &result);

    // Fallo desde RC
    if (rc != 0)
    {
        std::cerr << "Error getaddrinfo " << gai_strerror(rc) << std::endl;
        return -1;
    }

    int sd = socket(result->ai_family, result->ai_socktype, 0);

    // Metemos el addr en el socket
    bind(sd, result->ai_addr, result->ai_addrlen);

    int maxThreads = 5;
    for (int i = 0; i < maxThreads; i++)
    {
        MessageThread *message = new MessageThread(sd, i);
        std::thread([&message]() {message->Crear_mensaje(); delete message; }).detach();
    }

    // El thread principal cierra el servidor cuando reciba una q
    std::string q;
    do
    {
        std::cin >> q;
    } while (q != "q");

    // Cerramos todo
    close(sd);

    return 0;
}