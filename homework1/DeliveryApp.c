#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "DeliveryApp.h"
#include "OrderMenu.h"

////////////////////////////////////////////////////////////////////////////////
// begin define State methods
////////////////////////////////////////////////////////////////////////////////
struct State
{
    struct DeliveryApp *app;
    void (*printMenu)(struct State *state);
    int bRunning;
};

struct State *createStateMainMenu(struct DeliveryApp *app);
struct State *createStateShopList(struct DeliveryApp *app);
struct State *createStateShopMenu(struct DeliveryApp *app);
struct State *createStateExit(struct DeliveryApp *app);
void destroyState(struct State *state);

// void printMenu(struct DeliveryApp *app);
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
    struct Shop **shops;
    int nShops;
    struct State *state;
};

struct DeliveryApp *createDeliveryApp(struct Shop *shops[], int nShops)
{
    struct DeliveryApp *app = (struct DeliveryApp *)malloc(sizeof(struct DeliveryApp));
    if (app == NULL)
    {
        return NULL;
    }

    app->shops = shops;
    app->nShops = nShops;
    app->state = createStateMainMenu(app);
    return app;
}

void destroyDeliveryApp(struct DeliveryApp *app)
{
    destroyState(app->state);
    free(app);
}

void runDeliveryApp(struct DeliveryApp *app)
{
    printMenu(app);
    while (isRunning(app))
    {
        printMenu(app);
    }
}

int isRunning(struct DeliveryApp *app)
{
    return app->state->bRunning;
}

void printMenu(struct DeliveryApp *app)
{
    app->state->printMenu(app->state);
}

void changeState(struct DeliveryApp *app, struct State *state)
{
    destroyState(app->state);
    app->state = state;
}

void showNumber(int n)
{
    printf("Total price: $%d\n", n);
}

void countDown(int n)
{
    if (n <= 0)
    {
        return;
    }

    for (int i = n; i > 0; i--)
    {
        printf("Delivery in %d seconds\n", i);
        sleep(1);
    }

    printf("Delivery arrived\n");
}

void *threadShowNumber(void *arg)
{
    int n = *(int*)arg;
    showNumber(n);
    pthread_exit(NULL);
}

////////////////////////////////////////////////////////////////////////////////

struct State *createStateMainMenu(struct DeliveryApp *app)
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

struct State *createStateShopList(struct DeliveryApp *app)
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

struct State *createStateShopMenu(struct DeliveryApp *app)
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

struct State *createStateExit(struct DeliveryApp *app)
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
    struct DeliveryApp *app = state->app;
    int input;

    printf("1. shop list\n");
    printf("2. order\n");
    printf("3. exit\n");

    scanf(" %d", &input);
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
    struct DeliveryApp *app = state->app;
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

    changeState(app, createStateMainMenu(app));
}

void printShopMenu(struct State *state)
{
    struct DeliveryApp *app = state->app;
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

    scanf(" %d", &input);
    if (input < 1 || input > app->nShops)
    {
        changeState(app, createStateMainMenu(app));
        return;
    }

    OrderMenuPtr orderMenu = createOrderMenu(app->shops[input-1]);
    int ret = showOrderMenu(orderMenu);
    if (ret == STATUS_CONFIRM)
    {
        // asynchronously show total price
        pthread_t thread;
        int n = getTotalPrice(orderMenu);
        ret = pthread_create(&thread, NULL, threadShowNumber, (void*)&n);
        if (ret != 0)
        {
            fprintf(stderr, "pthread_create failed\n");
        }

        // count down delivery time
        countDown(orderMenu->shop->distance);
    }
    destroyOrderMenu(orderMenu);
    changeState(app, createStateMainMenu(app));
}

void printOrderMenu(struct State *state)
{
    struct DeliveryApp *app = state->app;
    changeState(app, createStateMainMenu(app));
    return;
}

void printExit(struct State *state)
{
    state->bRunning = 0;
    return;
}
