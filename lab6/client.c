#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

// client <ip> <port> <deposit/withdraw> <amount> <times>
int main(int argc, char const *argv[])
{
    const char *ip, *operation;
    int port, amount, time;
    struct sockaddr_in sa;

    if (argc != 6)
    {
        printf("Usage: %s <ip> <port> <deposit/withdraw> <amount> <time>\n", argv[0]);
        return EXIT_FAILURE;
    }

    ip = argv[1];
    port = atoi(argv[2]);
    operation = argv[3];
    amount = atoi(argv[4]);
    time = atoi(argv[5]);

    // validate ip
    int result = inet_pton(AF_INET, ip, &(sa.sin_addr));
    if (result != 1)
    {
        printf("Invalid IPv4 network address: %s (inet_pton returns %d)\n", ip, result);
        return EXIT_FAILURE;
    }

    // validate port
    if (port <= 0 || port > 65535)
    {
        printf("Invalid port: %d\n", port);
        return EXIT_FAILURE;
    }

    // validate operation
    if (strcmp(operation, "deposit") != 0 && strcmp(operation, "withdraw") != 0)
    {
        printf("Invalid operation: %s\n", operation);
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
    printf("ip: %s\n", ip);
    printf("port: %d\n", port);
    printf("operation: %s\n", operation);
    printf("amount: %d\n", amount);
    printf("time: %d\n", time);
#endif

    return EXIT_SUCCESS;
}
