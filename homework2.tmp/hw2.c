#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#include "Shop.h"
#include "DeliveryManager.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define IP "127.0.0.1"
#define MAX_CLIENTS 4096
int serverSocket = -1;

void sigintHandler(int signum);
void sigchldHandler(int signum);

void printShop(struct Shop *shop);

int main(int argc, char const *argv[])
{
    in_port_t port;
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t clientAddressSize = sizeof(clientAddress);
    int option = 1;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    port = atoi(argv[1]);

    struct Shop *shops[3];
    shops[0] = createShop("Dessert shop", 3);
    addMenuItem(shops[0], "cookie", 60);
    addMenuItem(shops[0], "cake", 80);
    shops[1] = createShop("Beverage shop", 5);
    addMenuItem(shops[1], "tea", 40);
    addMenuItem(shops[1], "boba", 70);
    shops[2] = createShop("Diner", 8);
    addMenuItem(shops[2], "fried-rice", 120);
    addMenuItem(shops[2], "Egg-drop-soup", 50);

    // signal(SIGINT, sigintHandler);
    signal(SIGCHLD, sigchldHandler);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0)
    {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(IP);
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

    DeliveryManagerPtr manager = createDeliveryManager(serverSocket, shops, ARRAY_SIZE(shops));
    runDeliveryManager(manager);
    destroyDeliveryManager(manager);

    if (close(serverSocket) < 0)
    {
        perror("close(serverSocket)");
        exit(EXIT_FAILURE);
    }

    destroyShop(shops[0]);
    destroyShop(shops[1]);
    destroyShop(shops[2]);
    return 0;
}

void sigintHandler(int signum)
{
    // if (serverSocket != -1)
    //     close(serverSocket);
}

void sigchldHandler(int signum)
{
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void printShop(struct Shop *shop)
{
    if (shop == NULL)
    {
        return;
    }

    printf("Shop: %s\n", shop->name);
    printf("Distance: %d\n", shop->distance);
    printf("Menu:\n");
    struct MenuItem *current = shop->menu;
    while (current != NULL)
    {
        printf("%s: $%d\n", current->name, current->price);
        current = current->next;
    }
}
