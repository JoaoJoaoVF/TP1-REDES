#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <math.h>

#define MAX_SIZE 100

typedef struct
{
    double latitude;
    double longitude;
} Coordinate;

Coordinate coordCli = {-19.939015762357613, -43.939794194394366};

void create_and_connect_socket(sa_family_t family, char *ipAddress, int port)
{
    // Declares variables for the socket descriptor, server address structure, and its size.
    int clientSocket;
    struct sockaddr_storage serverAddress;
    socklen_t addr_size;

    // Creates a socket:
    // - "family": Specifies the address family (AF_INET for IPv4 or AF_INET6 for IPv6).
    // - SOCK_STREAM: Indicates a TCP socket.
    // - 0: Default protocol (usually IPPROTO_TCP for TCP).
    clientSocket = socket(family, SOCK_STREAM, 0);

    // Error handling for socket creation
    if (clientSocket == -1)
    {
        perror("Erro ao criar o socket");
        exit(1);
    }

    // Configuring the address structure based on family (IPv4 or IPv6)
    memset(&serverAddress, 0, sizeof(serverAddress));
    if (family == AF_INET)
    {
        ((struct sockaddr_in *)&serverAddress)->sin_family = family;
        ((struct sockaddr_in *)&serverAddress)->sin_port = htons(port);
        inet_pton(family, ipAddress, &(((struct sockaddr_in *)&serverAddress)->sin_addr));
        addr_size = sizeof(struct sockaddr_in);
    }
    else if (family == AF_INET6)
    {
        ((struct sockaddr_in6 *)&serverAddress)->sin6_family = family;
        ((struct sockaddr_in6 *)&serverAddress)->sin6_port = htons(port);
        inet_pton(family, ipAddress, &(((struct sockaddr_in6 *)&serverAddress)->sin6_addr));
        addr_size = sizeof(struct sockaddr_in6);
    }

    // Attempts to connect the client socket to the server address:
    // - clientSocket: The socket descriptor created earlier.
    // - (struct sockaddr *)&serverAddress: The server address structure.
    // - addr_size: The size of the address structure.
    if (connect(clientSocket, (struct sockaddr *)&serverAddress, addr_size) == -1)
    {
        perror("Erro ao conectar ao servidor");
        exit(1);
    }

    // Sends the "coordCli" structure (containing client coordinates) to the server.
    send(clientSocket, &coordCli, sizeof(coordCli), 0);

    // Receiving the server's response
    char serverResponse[MAX_SIZE];
    recv(clientSocket, serverResponse, MAX_SIZE, 0);

    // Processing the server's response and displaying updates
    if (strcmp(serverResponse, "Motorista a caminho") == 0)
    {
        double distance;
        while (1)
        {
            recv(clientSocket, &distance, sizeof(distance), 0);
            if (distance == 0)
            {
                printf("O motorista chegou!\n");
                exit(0);
            }
            else
            {
                printf("Motorista à %.0lf metros\n", distance);
            }
        }
    }
    else
    {
        printf("Nao foi encontrado um motorista\n");
    }

    // Closes the client socket.
    close(clientSocket);
}

// Main function of the program.
int main(int argc, char **argv)
{
    // Reading server data and validating input
    if (argc != 4)
    {
        printf("Número inválido de argumentos\n");
        exit(1);
    }
    char *ipVersion = argv[1];
    char *ipAddress = argv[2];
    int port = atoi(argv[3]);

    if (strcmp(ipVersion, "ipv4") != 0 && strcmp(ipVersion, "ipv6") != 0)
    {
        printf("Versão de IP inválida\n");
        exit(1);
    }

    // Main loop for user interaction:
    // - Prints a menu for the user to select an action (exit or request ride).
    // - Based on the user's choice, calls the appropriate function (IPv4 or IPv6) to handle the connection and communication with the server.
    while (1)
    {
        printf("0 - Sair\n");
        printf("1 - Solicitar corrida\n");

        int clientAction;
        if (scanf("%d", &clientAction) != 1)
        {
            fprintf(stderr, "Erro na entrada\n");
            exit(1);
        }

        if (clientAction == 0)
        {
            break; // Exit the loop
        }
        else if (clientAction == 1)
        {
            // Request ride based on IP version
            if (strcmp(ipVersion, "ipv4") == 0)
            {
                create_and_connect_socket(AF_INET, ipAddress, port);
            }
            else if (strcmp(ipVersion, "ipv6") == 0)
            {
                create_and_connect_socket(AF_INET6, ipAddress, port);
            }
            else
            {
                printf("Versão de IP inválida\n");
                exit(1);
            }
        }
        else
        {
            printf("Opção inválida! Digite 1 para solicitar uma corrida ou 0 para sair\n");
        }
    }

    return 0;
}