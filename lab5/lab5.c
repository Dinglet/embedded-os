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

#define MAX_CLIENTS 3

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
    int option = 1;

    signal(SIGINT, sigint_handler);
    signal(SIGCHLD, handler);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0)
    {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port = htons(4444);

    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        perror("bind()");
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocket, MAX_CLIENTS) < 0)
    {
        perror("listen()");
        exit(EXIT_FAILURE);
    }

    /* Now we can accept incoming connections one
        at a time using accept(2). */
    while ((clientSocket = accept(serverSocket, (struct sockaddr *) &clientAddress, &clientAddressSize)) >= 0)
    {
        pid_t child = fork();
        if (child == -1)
        {
            perror("fork()");
            close(clientSocket);
            continue;
        }
        else if (child > 0)
        {
            // parent process
            printf("Train ID: %d\n", child);
            close(clientSocket);
            continue;
        }
        else
        {
            // child process
            if (close(serverSocket) < 0)
            {
                perror("close(serverSocket)");
                exit(EXIT_FAILURE);
            }

            // duplicate the client socket file descriptor
            // the file descriptor STDOUT_FILENO is adjusted so that it refers to the client socket descriptor
            dup2(clientSocket, STDOUT_FILENO);
            if (close(clientSocket) < 0)
            {
                perror("close(clientSocket)");
                exit(EXIT_FAILURE);
            }
            // // code here can print different process IDs to different sockets
            // for (int i = 0; i < 10; i++)
            // {
            //     printf("pid: %d\n", getpid());
            //     sleep(1);
            // }
            execlp("sl", "sl", "-l", NULL);
            exit(EXIT_SUCCESS);
        }
    }

    if (close(serverSocket) < 0)
    {
        perror("close(serverSocket)");
        exit(EXIT_FAILURE);
    }

    return 0;
}
