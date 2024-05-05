#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <math.h>

typedef struct
{
    double latitude;
    double longitude;
} Coordinate;

Coordinate coordServ = {-19.9227, -43.9541};

// Function to calculate the distance between two coordinates using the Haversine formula
double haversine(double lat1, double lon1, double lat2, double lon2)
{
    double dLat = (lat2 - lat1) * M_PI / 180.0;
    double dLon = (lon2 - lon1) * M_PI / 180.0;

    lat1 = lat1 * M_PI / 180.0;
    lat2 = lat2 * M_PI / 180.0;

    double dist = pow(sin(dLat / 2), 2) + pow(sin(dLon / 2), 2) * cos(lat1) * cos(lat2);
    double rad = 6371;
    double c = 2 * asin(sqrt(dist));
    double d = rad * c * 1000.0;

    return d;
}

// Function to handle client connection and communication
void handle_client(int clientSocket)
{
    Coordinate coordCli;

    // recv() receives data from the client and stores it in coordCli.
    // Returns the number of bytes received or 0 if the connection was closed.
    int bytes_received = recv(clientSocket, &coordCli, sizeof(coordCli), 0);

    // Check if client disconnected
    if (bytes_received == 0)
    {
        printf("Cliente desconectou.\n");
        close(clientSocket);
        return;
    }

    // Calculate distance between server and client
    double distance = haversine(coordServ.latitude, coordServ.longitude, coordCli.latitude, coordCli.longitude);

    int option[2];
    printf("0 - Recusar\n");
    printf("1 - Aceitar\n");
    scanf("%d", option);

    if (option[0] == 1) // If request is accepted
    {
        // Send confirmation message to client
        char message[] = "Motorista a caminho";
        send(clientSocket, message, strlen(message) + 1, 0);

        // Send distance updates to client every 2 seconds
        while (distance > 0)
        {
            send(clientSocket, &distance, sizeof(distance), 0);
            distance -= 400;
            sleep(2);
        }

        // Envia uma mensagem de chegada para o cliente
        distance = 0;
        send(clientSocket, &distance, sizeof(distance), 0);
        printf("O motorista chegou!\n");
    }
    else
    {
        // Send arrival message to client
        char message[] = "Não foi encontrado um motorista.";
        send(clientSocket, message, strlen(message) + 1, 0);
        close(clientSocket);
        // TODO libera a porta do server
        printf("Aguardando solicitação.\n");
    }

    // Close client socket
    close(clientSocket);
}

// Function to create server socket and listen for connections
void create_server_socket_and_listen(sa_family_t family, int port)
{
    // Declares variables for the socket descriptor, server address structure, and its size.
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddress, clientAddress;
    struct sockaddr_in6 serverAddress6, clientAddress6;
    socklen_t clientAddressLength;

    // Creates a socket:
    // - "family": Specifies the address family (AF_INET for IPv4 or AF_INET6 for IPv6).
    // - SOCK_STREAM: Indicates a TCP socket.
    // - 0: Default protocol (usually IPPROTO_TCP for TCP).
    serverSocket = socket(family, SOCK_STREAM, 0);

    // Error handling for socket creation
    if (serverSocket == -1)
    {
        perror("Erro ao criar o socket");
        exit(1);
    }

    // Configuring the address structure based on family (IPv4 or IPv6)
    if (family == AF_INET) // IPv4
    {
        memset(&serverAddress, 0, sizeof(serverAddress));
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = INADDR_ANY;
        serverAddress.sin_port = htons(port);

        // Bind socket to address
        if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
        {
            perror("Erro ao associar o socket ao endereço");
            exit(1);
        }
    }
    else if (family == AF_INET6) // IPv6
    {
        memset(&serverAddress6, 0, sizeof(serverAddress6));
        serverAddress6.sin6_family = AF_INET6;
        if (inet_pton(AF_INET6, "::", &(serverAddress6.sin6_addr)) != 1)
        {
            perror("Erro ao configurar o endereço do servidor");
            exit(1);
        }
        serverAddress6.sin6_port = htons(port);

        // Bind socket to address
        if (bind(serverSocket, (struct sockaddr *)&serverAddress6, sizeof(serverAddress6)) == -1)
        {
            perror("Erro ao associar o socket ao endereço");
            exit(1);
        }
    }

    // Listen for connections (maximum of 5 queued connections)
    if (listen(serverSocket, 5) == -1)
    {
        perror("Erro ao ouvir por conexões");
        exit(1);
    }

    printf("Aguardando solicitação.\n");

    // Main loop for user interaction:
    // Accept a connection
    // Handle client communication
    while (1)
    {
        if (family == AF_INET)
        {
            clientAddressLength = sizeof(clientAddress);
            clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLength);
        }
        else if (family == AF_INET6)
        {
            clientAddressLength = sizeof(clientAddress6);
            clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress6, &clientAddressLength);
        }

        if (clientSocket == -1)
        {
            perror("Erro ao aceitar conexão");
            continue;
        }

        handle_client(clientSocket);
    }

    close(serverSocket);
}

int main(int argc, char **argv)
{
    // Check for correct number of arguments
    if (argc != 3)
    {
        printf("Número inválido de argumentos\n");
        exit(1);
    }

    char *ipVersion = argv[1];
    int port = atoi(argv[2]);

    // Validate IP version
    if (strcmp(ipVersion, "ipv4") != 0 && strcmp(ipVersion, "ipv6") != 0)
    {
        printf("Versão de IP inválida\n");
        exit(1);
    }

    // Create server socket and listen for connections based on IP version
    if (strcmp(ipVersion, "ipv4") == 0)
    {
        create_server_socket_and_listen(AF_INET, port);
    }
    else if (strcmp(ipVersion, "ipv6") == 0)
    {
        create_server_socket_and_listen(AF_INET6, port);
    }

    return 0;
}