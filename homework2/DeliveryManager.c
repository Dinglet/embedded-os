#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <pthread.h>

#include "DeliveryManager.h"
#include "DeliveryApp.h"

struct DeliveryManager
{
    int socket;
    ShopPtr *shops;
    int nShops;
};

static void *deliverAppHandler(void *arg);

DeliveryManagerPtr createDeliveryManager(int socket, ShopPtr shops[], int nShops)
{
    DeliveryManagerPtr manager = (DeliveryManagerPtr)malloc(sizeof(DeliveryManager));
    if (manager == NULL)
    {
        return NULL;
    }

    manager->socket = socket;
    manager->shops = shops;
    manager->nShops = nShops;
    return manager;
}

void destroyDeliveryManager(DeliveryManagerPtr manager)
{
    if (manager == NULL)
    {
        return;
    }

    free(manager);
}

void runDeliveryManager(DeliveryManagerPtr manager)
{
    int clientSocket;

    while ((clientSocket = accept(manager->socket, NULL, NULL)) >= 0)
    {
        pthread_t thread;
        // pthread_attr_t attr;

        // pthread_attr_init(&attr);
        // pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

        DeliveryAppPtr app = createDeliveryApp(clientSocket, manager->shops, manager->nShops);
        pthread_create(&thread, NULL, deliverAppHandler, app);
        pthread_join(thread, NULL);
    }
    if (clientSocket < 0)
    {
        perror("accept()");
        exit(EXIT_FAILURE);
    }
}

void *deliverAppHandler(void *arg)
{
    DeliveryAppPtr app = (DeliveryAppPtr)arg;
    runDeliveryApp(app);
    close(getSocket(app));
    destroyDeliveryApp(app);
    pthread_exit(NULL);
}