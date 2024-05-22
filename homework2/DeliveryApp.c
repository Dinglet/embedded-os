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
    destroyCart(app->cart);
    free(app);
}

void runDeliveryApp(DeliveryAppPtr app)
{
    char buffer[BUFFER_SIZE] = {0};
    char *p = NULL;
    int count = 0;

    app->bRunning = 1;

    while (app->bRunning && (count = recv(app->clientSocket, buffer, BUFFER_SIZE, 0)) > 0)
    {
        for (int i = 0; i < count; i++)
        {
            if (buffer[i] == '\n')
            {
                buffer[i] = '\0';
                break;
            }
        }

        printf("(%d) Received: %s\n", app->id, buffer);
        CommandPtr command = createCommand(buffer);
        if (command == NULL)
        {
            continue;
        }

        switch (command->type)
        {
        case kShopList:
            // print shop list
            buffer[0] = '\0';
            count = 0;

            for (int i = 0; i < app->nShops; i++)
            {
                ShopPtr shop = app->shops[i];
                MenuItemPtr menuItem = shop->menu;

                count += sprintf(buffer + count, "%s:%dkm\n", shop->name, shop->distance);
                count += sprintf(buffer + count, "- ");
                while (menuItem)
                {
                    count += sprintf(buffer + count, "%s:$%d|", menuItem->name, menuItem->price);
                    menuItem = menuItem->next;
                }
                buffer[count-1] = '\n';
            }

            send(app->clientSocket, buffer, BUFFER_SIZE, 0);
            printf("(%d) Sent: %s\n", app->id, buffer);
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

            memset(buffer, 0, BUFFER_SIZE);
            count = 0;

            sscanf(p, "%s %d", meal, &quantity);
            addItem(app->cart, meal, quantity);

            // meals in the cart
            MenuItemPtr current = app->cart->shop->menu;
            for (int i = 0; i < app->cart->nItems; i++)
            {
                if (app->cart->quantities[i] > 0)
                {
                    count += sprintf(buffer + count, "%s %d|", current->name, app->cart->quantities[i]);
                }
                current = current->next;
            }
            if (count > 1)
                buffer[count-1] = '\n';

            send(app->clientSocket, buffer, BUFFER_SIZE, 0);
            printf("(%d) Sent: %s\n", app->id, buffer);
            break;
        case kConfirm:
            if (app->cart == NULL)
            {
                snprintf(buffer, BUFFER_SIZE, "Please order some meals\n");
                send(app->clientSocket, buffer, BUFFER_SIZE, 0);
                printf("(%d) Sent: %s\n", app->id, buffer);
                break;
            }
            snprintf(buffer, BUFFER_SIZE, "Please wait a few minutes...\n");
            send(app->clientSocket, buffer, BUFFER_SIZE, 0);
            printf("(%d) Sent: %s\n", app->id, buffer);

            sleep(app->cart->shop->distance);

            snprintf(buffer, BUFFER_SIZE, "Delivery has arrived and you need to pay %d$\n", getTotalPrice(app->cart));
            send(app->clientSocket, buffer, BUFFER_SIZE, 0);
            printf("(%d) Sent: %s\n", app->id, buffer);
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
