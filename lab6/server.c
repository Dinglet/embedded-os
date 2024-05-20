#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <pthread.h>
#include <sys/sem.h>

int balance = 0;
int serverSocket = -1;

#define ADDRESS "127.0.0.1"
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

int sem_init(int key, int value);
int sem_close(int key);

int mySemid = -1;
int sem_wait(int semid);
int sem_signal(int semid);

void sigintHandler(int signum)
{
    if (serverSocket != -1)
        close(serverSocket);
    sem_close(mySemid);
}

void sigchldHandler(int signum)
{
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void *connectionHandler(void *arg)
{
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

        sem_wait(mySemid);
        balance += (transactionType == DEPOSIT) ? amount : -amount;
        printf("After %s: %d\n", strTransaction, balance);
        sem_signal(mySemid);
    }
    if (count < 0)
    {
        perror("recv()");
        pthread_exit(NULL);
    }
    else if (count == 0)
    {
        // printf("Client disconnected\n");
    }

    close(clientSocket);

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    in_port_t port;
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t clientAddressSize = sizeof(clientAddress);
    int option = 1;
    int clientSocket;

    if (argc != 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    port = atoi(argv[1]);

    mySemid = sem_init(0x1234, 1);
    signal(SIGINT, sigintHandler);
    signal(SIGHUP, sigintHandler);
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
    serverAddress.sin_port = htons(port);

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

        pthread_attr_t attr;
        pthread_t thread;

        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        if (pthread_create(&thread, &attr, connectionHandler, (void *) &clientSocket) < 0)
        {
            perror("pthread_create()");
            exit(EXIT_FAILURE);
        }

        pthread_attr_destroy(&attr);
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

    sem_close(mySemid);

    return 0;
}

int sem_init(int key, int value)
{
    int semid = semget(key, 1, IPC_CREAT | IPC_EXCL | 0600);
    if (semid < 0)
    {
        perror("semget()");
        return -1;
    }

    if (semctl(semid, 0, SETVAL, value) < 0)
    {
        perror("semctl()");
        return -1;
    }

    return semid;
}

int sem_close(int semid)
{
    if (semctl(semid, 0, IPC_RMID) < 0)
    {
        perror("semctl()");
        return -1;
    }

    return 0;
}

int sem_wait(int semid)
{
    struct sembuf sops; /* the operation parameters */
    sops.sem_num = 0; /* access the 1st (and only) sem in the array */
    sops.sem_op = -1; /* wait..*/
    sops.sem_flg = 0; /* no special options needed */

    if (semop(semid, &sops, 1) < 0)
    {
        perror("semop() in wait()");
        return -1;
    }
    else
    {
        return 0;
    }
}

int sem_signal(int semid)
{
    struct sembuf sops; /* the operation parameters */
    sops.sem_num = 0; /* the 1st (and only) sem in the array */
    sops.sem_op = 1; /* signal */
    sops.sem_flg = 0; /* no special options needed */

    if (semop(semid, &sops, 1) < 0)
    {
        perror("semop() in signal()");
        return -1;
    }
    else
    {
        return 0;
    }
}
