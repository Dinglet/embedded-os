#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>

#include "DeliveryApp.h"
#include "OrderMenu.h"
#include "common.h"
#include "Command.h"

#define DEVICE_DISTANCE "/dev/leds"
#define DEVICE_PRICE "/dev/segments"

////////////////////////////////////////////////////////////////////////////////
// begin define State methods
////////////////////////////////////////////////////////////////////////////////
struct State
{
    DeliveryAppPtr app;
    void (*printMenu)(struct State *state);
    int bRunning;
};

struct State *createStateMainMenu(DeliveryAppPtr app);
struct State *createStateShopList(DeliveryAppPtr app);
struct State *createStateShopMenu(DeliveryAppPtr app);
struct State *createStateExit(DeliveryAppPtr app);
void destroyState(struct State *state);

// void printMenu(DeliveryAppPtr app);
void printMainMenu(struct State *state);
void printShopList(struct State *state);
void printShopMenu(struct State *state);
void printOrderMenu(struct State *state);
void printExit(struct State *state);
////////////////////////////////////////////////////////////////////////////////
// end define State methods
////////////////////////////////////////////////////////////////////////////////

struct DeliveryApp
{
    int clientSocket;
    struct Shop **shops;
    int nShops;
    struct State *state;
    int bRunning;
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
    app->state = createStateMainMenu(app);
    return app;
}

void destroyDeliveryApp(DeliveryAppPtr app)
{
    destroyState(app->state);
    free(app);
}

void runDeliveryApp(DeliveryAppPtr app)
{
    char buffer[BUFFER_SIZE] = {0};
    int count = 0, i = 0;

    app->bRunning = 1;

    while (app->bRunning && (count = recv(app->clientSocket, buffer, BUFFER_SIZE, 0)) > 0)
    {
        for (i = 0; i < count; i++)
        {
            if (buffer[i] == '\n')
            {
                buffer[i] = '\0';
                break;
            }
        }

        printf("Received: %s\n", buffer);
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
            break;
        case kOrder:

            break;
        case kConfirm:

            break;
        case kCancel:
            printf("Client cancel the order\n");
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

int isRunning(DeliveryAppPtr app)
{
    return app->state->bRunning;
}

void printMenu(DeliveryAppPtr app)
{
    app->state->printMenu(app->state);
}

void changeState(DeliveryAppPtr app, struct State *state)
{
    destroyState(app->state);
    app->state = state;
}

void showNumber(int n)
{
    int fd = -1;
    char strNumber[11] = {0}, *p = 0;
    if (n < 0)
    {
        return;
    }

    fd = open(DEVICE_PRICE, O_WRONLY);
    if (fd == -1)
    {
        fprintf(stderr, "open %s failed, use stdout instead\n", DEVICE_PRICE);
        printf("Total price: $%d\n", n);
        return;
    }

    sprintf(strNumber, "%d", n);
    p = strNumber;
    while (*p)
    {
        // printf("Writing %d for price\n", *p);
        write(fd, p, sizeof(char));
        usleep(500000);
        p++;
    }

    close(fd);
}

void countDown(int n)
{
    int i = 0, fd = -1;
    if (n <= 0)
    {
        return;
    }

    fd = open(DEVICE_DISTANCE, O_WRONLY);
    if (fd == -1)
    {
        fprintf(stderr, "open %s failed, use stdout instead\n", DEVICE_DISTANCE);

        for (i = n; i > 0; i--)
        {
            printf("Delivery in %d seconds\n", i);
            sleep(1);
        }

        printf("Delivery arrived\n");
        return;
    }

    for (i = n; i > 0; i--)
    {
        write(fd, &i, sizeof(int));
        sleep(1);
    }
    i = 0;
    write(fd, &i, sizeof(int));
    close(fd);
}

void *threadShowNumber(void *arg)
{
    int n = *(int*)arg;
    showNumber(n);
    pthread_exit(NULL);
}

////////////////////////////////////////////////////////////////////////////////

struct State *createStateMainMenu(DeliveryAppPtr app)
{
    struct State *state = (struct State *)malloc(sizeof(struct State));
    if (state == NULL)
    {
        return NULL;
    }

    state->app = app;
    state->printMenu = printMainMenu;
    state->bRunning = 1;
    return state;
}

struct State *createStateShopList(DeliveryAppPtr app)
{
    struct State *state = (struct State *)malloc(sizeof(struct State));
    if (state == NULL)
    {
        return NULL;
    }

    state->app = app;
    state->printMenu = printShopList;
    state->bRunning = 1;
    return state;
}

struct State *createStateShopMenu(DeliveryAppPtr app)
{
    struct State *state = (struct State *)malloc(sizeof(struct State));
    if (state == NULL)
    {
        return NULL;
    }

    state->app = app;
    state->printMenu = printShopMenu;
    state->bRunning = 1;
    return state;
}

struct State *createStateExit(DeliveryAppPtr app)
{
    struct State *state = (struct State *)malloc(sizeof(struct State));
    if (state == NULL)
    {
        return NULL;
    }

    state->app = app;
    state->printMenu = printExit;
    state->bRunning = 1;
    return state;
}

void destroyState(struct State *state)
{
    free(state);
}

void printMainMenu(struct State *state)
{
    DeliveryAppPtr app = state->app;
    int input;

    printf("1. shop list\n");
    printf("2. order\n");
    // printf("3. exit\n");

    while (getIntegerInput(&input) != 0 || input < 1 || input > 3)
    {
        printf("Invalid input!\n");
    }

    switch (input)
    {
    case 1:
        changeState(app, createStateShopList(app));
        break;
    case 2:
        changeState(app, createStateShopMenu(app));
        break;
    case 3:
        changeState(app, createStateExit(app));
        break;
    default:
        break;
    }
}

void printShopList(struct State *state)
{
    DeliveryAppPtr app = state->app;
    int input;

    if (app->nShops <= 0)
    {
        printf("No shop available\n");
        changeState(app, createStateMainMenu(app));
        return;
    }

    for (int i = 0; i < app->nShops; i++)
    {
        printf("%s: %dkm\n", app->shops[i]->name, app->shops[i]->distance);
    }

    waitForKey();

    changeState(app, createStateMainMenu(app));
}

void printShopMenu(struct State *state)
{
    DeliveryAppPtr app = state->app;
    int input;
    if (app->nShops <= 0)
    {
        printf("No shop available\n");
        changeState(app, createStateMainMenu(app));
        return;
    }

    printf("Please choose from 1~%d\n", app->nShops);
    for (int i = 0; i < app->nShops; i++)
    {
        printf("%d. %s\n", i+1, app->shops[i]->name);
    }

    while (getIntegerInput(&input) != 0 || input < 1 || input > app->nShops)
    {
        printf("Invalid input!\n");
    }

    OrderMenuPtr orderMenu = createOrderMenu(app->shops[input-1]);
    int ret = showOrderMenu(orderMenu);
    int n = getTotalPrice(orderMenu->cart);
    if (ret == STATUS_CONFIRM && n > 0)
    {
        // asynchronously show total price
        pthread_t thread;
        ret = pthread_create(&thread, NULL, threadShowNumber, (void*)&n);
        if (ret != 0)
        {
            fprintf(stderr, "pthread_create failed\n");
        }

        printf("Please wait for a few minutes...\n");

        // count down delivery time
        countDown(orderMenu->shop->distance);

        printf("please pick up your meal\n");
        waitForKey();
    }
    destroyOrderMenu(orderMenu);
    changeState(app, createStateMainMenu(app));
}

void printOrderMenu(struct State *state)
{
    DeliveryAppPtr app = state->app;
    changeState(app, createStateMainMenu(app));
    return;
}

void printExit(struct State *state)
{
    state->bRunning = 0;
    return;
}
