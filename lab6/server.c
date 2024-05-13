#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <pthread.h>

int balance = 0;
int serverSocket = -1;

#define ADDRESS "127.0.0.1"
#define PORT 8888
#define MAX_CLIENTS 4096

typedef enum TransactionType
{
    DEPOSIT,
    WITHDRAW,
    INVALID = -1
} TransactionType;

typedef struct Transaction
{
    enum TransactionType transactionType;
    int amount;
} Transaction;

void sigintHandler(int signum)
{
    if (serverSocket != -1)
        close(serverSocket);
}

void sigchldHandler(int signum)
{
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void *connectionHandler(void *arg)
{
    pthread_detach(pthread_self());
    int i = 0;

    int clientSocket = *(int *) arg;
    Transaction transaction;
    void *ptr = &transaction;
    size_t count = 0;
    size_t len = 0;

    while ((count = recv(clientSocket, ptr + len, sizeof(transaction) - len, 0)) > 0)
    {
        len += count;
        if (len < sizeof(transaction))
            continue;;
        len = 0;

        // process transaction
        TransactionType transactionType = transaction.transactionType;
        int amount = transaction.amount;
        char *strTransaction = (transactionType == DEPOSIT) ? "deposit" : (transactionType == WITHDRAW) ? "withdraw" : "invalid";

        balance += (transactionType == DEPOSIT) ? amount : -amount;
        printf("After %s: %d\n", strTransaction, balance);
    }
    if (count < 0)
    {
        perror("recv()");
        pthread_exit(NULL);
    }

    close(clientSocket);

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t clientAddressSize = sizeof(clientAddress);
    int option = 1;
    int clientSocket;

    signal(SIGINT, sigintHandler);
    signal(SIGCHLD, sigchldHandler);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0)
    {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(ADDRESS);
    serverAddress.sin_port = htons(PORT);

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
        // use pthread to handle each connection

        pthread_t thread;
        if (pthread_create(&thread, NULL, connectionHandler, (void *) &clientSocket) < 0)
        {
            perror("pthread_create()");
            exit(EXIT_FAILURE);
        }

        clientAddressSize = sizeof(clientAddress);
    }
    if (clientSocket < 0)
    {
        perror("accept()");
        exit(EXIT_FAILURE);
    }

    if (close(serverSocket) < 0)
    {
        perror("close(serverSocket)");
        exit(EXIT_FAILURE);
    }

    return 0;
}
