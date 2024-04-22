#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

int serverSocket = -1;
int clientSocket = -1;

void sigint_handler(int signum)
{
    if (serverSocket != -1)
        close(serverSocket);
}

void handler(int signum)
{
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main(int argc, char *argv[])
{
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t clientAddressSize = sizeof(clientAddress);

    signal(SIGINT, sigint_handler);
    signal(SIGCHLD, handler);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port = htons(4444);

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        perror("bind()");
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocket, 5) == -1)
    {
        perror("listen()");
        exit(EXIT_FAILURE);
    }

    /* Now we can accept incoming connections one
        at a time using accept(2). */

    clientSocket = accept(serverSocket, (struct sockaddr *) &clientAddress, &clientAddressSize);
    if (clientSocket == -1)
    {
        perror("accept()");
        exit(EXIT_FAILURE);
    }

    // duplicate the client socket file descriptor
    // the file descriptor STDOUT_FILENO is adjusted so that it refers to the client socket descriptor
    dup2(clientSocket, STDOUT_FILENO);
    close(clientSocket);
    execlp("sl", "sl", "-l", NULL);

    if (close(serverSocket) == -1)
    {
        perror("close()");
        exit(EXIT_FAILURE);
    }

    return 0;
}