#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

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

// client <IPv4 address> <port> <deposit/withdraw> <amount> <times>
int main(int argc, char const *argv[])
{
    const char *strAddress, *strTransaction;
    in_port_t port;
    int amount, time;
    struct sockaddr_in sa;
    int i;

    if (argc != 6)
    {
        printf("Usage: %s <IPv4 address> <port> <deposit/withdraw> <amount> <time>\n", argv[0]);
        return EXIT_FAILURE;
    }

    strAddress = argv[1];
    port = atoi(argv[2]);
    strTransaction = argv[3];
    amount = atoi(argv[4]);
    time = atoi(argv[5]);

    // validate IPv4 address and port
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    int result = inet_pton(AF_INET, strAddress, &(sa.sin_addr));
    if (result < 0)
    {
        perror("inet_pton()");
        return EXIT_FAILURE;
    }
    else if (result == 0)
    {
        printf("Invalid IPv4 network address: %s\n", strAddress);
        return EXIT_FAILURE;
    }
    // if (port <= 0 || port > 65535)
    // {
    //     printf("Invalid port: %d\n", port);
    //     return EXIT_FAILURE;
    // }
    sa.sin_port = htons(port);

    // validate transaction
    TransactionType transactionType = INVALID;
    if (strcmp(strTransaction, "deposit") == 0)
        transactionType = DEPOSIT;
    else if (strcmp(strTransaction, "withdraw") == 0)
        transactionType = WITHDRAW;
    else
    {
        printf("Invalid transaction: %s\n", strTransaction);
        return EXIT_FAILURE;
    }

    // validate amount
    if (amount <= 0)
    {
        printf("Invalid amount: %d\n", amount);
        return EXIT_FAILURE;
    }

    // validate time
    if (time <= 0)
    {
        printf("Invalid time: %d\n", time);
        return EXIT_FAILURE;
    }

#ifdef DEBUG
    printf("IPv4 address: %s\n", strAddress);
    printf("port: %d\n", port);
    printf("transaction: %s\n", strTransaction);
    printf("amount: %d\n", amount);
    printf("time: %d\n", time);
#endif

    Transaction transaction = {transactionType, amount};

    int server;
    if ((server = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket()");
        exit(EXIT_FAILURE);
    }
    if (connect(server, (struct sockaddr *) &sa, sizeof(sa)) < 0)
    {
        perror("connect()");
        exit(EXIT_FAILURE);
    }
    for (i = 0; i < time; i++)
    {
        // send transaction
        if (send(server, &transaction, sizeof(transaction), 0) < 0)
        {
            perror("send()");
            exit(EXIT_FAILURE);
        }
    }
    // close connection
    close(server);

    return EXIT_SUCCESS;
}
