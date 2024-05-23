#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>

#include "DeliveryApp.h"
#include "Shop.h"
#include "Cart.h"
#include "common.h"
#include "Command.h"

static int countApp = 0;
struct DeliveryApp
{
    int clientSocket;
    struct Shop **shops;
    int nShops;
    int bRunning;
    CartPtr cart;
    int id;
};

DeliveryAppPtr createDeliveryApp(int clientSocket, struct Shop *shops[], int nShops)
{
    DeliveryAppPtr app = (DeliveryAppPtr )malloc(sizeof(struct DeliveryApp));
    if (app == NULL)
    {
        return NULL;
    }

    app->clientSocket = clientSocket;
    app->shops = shops;
    app->nShops = nShops;
    app->bRunning = 0;
    app->cart = NULL;
    app->id = countApp++;
    return app;
}

void destroyDeliveryApp(DeliveryAppPtr app)
{
    if (app == NULL)
    {
        return;
    }

    destroyCart(app->cart);
    free(app);
}

void runDeliveryApp(DeliveryAppPtr app)
{
    char readBuffer[BUFFER_SIZE] = {0};
    char writeBuffer[BUFFER_SIZE] = {0};
    char *p = NULL;
    int count = 0;

    app->bRunning = 1;

    while (app->bRunning && (count = recv(app->clientSocket, readBuffer, BUFFER_SIZE, 0)) > 0)
    {
        for (int i = 0; i < count; i++)
        {
            if (readBuffer[i] == '\n')
            {
                readBuffer[i] = '\0';
                break;
            }
        }

        printf("(%d) Received: %s\n", app->id, readBuffer);
        CommandPtr command = createCommand(readBuffer);
        if (command == NULL)
        {
            continue;
        }

        switch (command->type)
        {
        case kShopList:
            // print shop list
            memset(writeBuffer, 0, BUFFER_SIZE);
            count = 0;

            for (int i = 0; i < app->nShops; i++)
            {
                ShopPtr shop = app->shops[i];
                MenuItemPtr menuItem = shop->menu;

                count += sprintf(writeBuffer + count, "%s:%dkm\n", shop->name, shop->distance);
                count += sprintf(writeBuffer + count, "- ");
                while (menuItem)
                {
                    count += sprintf(writeBuffer + count, "%s:$%d|", menuItem->name, menuItem->price);
                    menuItem = menuItem->next;
                }
                writeBuffer[count-1] = '\n';
            }

            send(app->clientSocket, writeBuffer, BUFFER_SIZE, 0);
            printf("(%d) Sent: %s\n", app->id, writeBuffer);
            break;
        case kOrder:
            // parse order "<meal> <quantity>"
            p = command->arg;
            // skip whitespace
            while (*p && isspace(*p))
            {
                p++;
            }
            if (app->cart == NULL)
            {
                // determine which shop according to the meal
                for (int i = 0; i < app->nShops; i++)
                {
                    ShopPtr shop = app->shops[i];
                    MenuItemPtr menuItem = shop->menu;
                    while (menuItem)
                    {
                        if (startWith(p, menuItem->name))
                        {
                            printf("(%d) Found %s in %s\n", app->id, menuItem->name, shop->name);
                            app->cart = createCart(shop);
                            break;
                        }
                        menuItem = menuItem->next;
                    }
                    if (app->cart != NULL)
                    {
                        break;
                    }
                }
            }

            // determine meal and quantity
            // check if the meal is in the shop
            char meal[256] = {0};
            int quantity = 0;

            sscanf(p, "%s %d", meal, &quantity);
            addItem(app->cart, meal, quantity);

            memset(writeBuffer, 0, BUFFER_SIZE);
            count = 0;
            // meals in the cart
            MenuItemPtr current = app->cart->shop->menu;
            for (int i = 0; i < app->cart->nItems; i++)
            {
                if (app->cart->quantities[i] > 0)
                {
                    count += sprintf(writeBuffer + count, "%s %d|", current->name, app->cart->quantities[i]);
                }
                current = current->next;
            }
            if (count > 1)
                writeBuffer[count-1] = '\n';

            send(app->clientSocket, writeBuffer, BUFFER_SIZE, 0);
            printf("(%d) Sent: %s\n", app->id, writeBuffer);
            break;
        case kConfirm:
            if (app->cart == NULL)
            {
                memset(writeBuffer, 0, BUFFER_SIZE);
                snprintf(writeBuffer, BUFFER_SIZE, "Please order some meals\n");
                send(app->clientSocket, writeBuffer, BUFFER_SIZE, 0);
                printf("(%d) Sent: %s\n", app->id, writeBuffer);
                break;
            }
            memset(writeBuffer, 0, BUFFER_SIZE);
            snprintf(writeBuffer, BUFFER_SIZE, "Please wait a few minutes...\n");
            send(app->clientSocket, writeBuffer, BUFFER_SIZE, 0);
            printf("(%d) Sent: %s\n", app->id, writeBuffer);

            sleep(app->cart->shop->distance);

            memset(writeBuffer, 0, BUFFER_SIZE);
            snprintf(writeBuffer, BUFFER_SIZE, "Delivery has arrived and you need to pay %d$\n", getTotalPrice(app->cart));
            send(app->clientSocket, writeBuffer, BUFFER_SIZE, 0);
            printf("(%d) Sent: %s\n", app->id, writeBuffer);

            app->bRunning = 0;
            break;
        case kCancel:
            app->bRunning = 0;
            break;
        default:
            break;
        }

        destroyCommand(command);
    }
}

int getSocket(DeliveryAppPtr app)
{
    return app->clientSocket;
}
