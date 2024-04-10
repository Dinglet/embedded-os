#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "DeliveryApp.h"

////////////////////////////////////////////////////////////////////////////////
// begin define State methods
////////////////////////////////////////////////////////////////////////////////
struct State *createStateMainMenu(struct DeliveryApp *app);
struct State *createStateShopList(struct DeliveryApp *app);
struct State *createStateOrderMenu(struct DeliveryApp *app);
struct State *createStateExit(struct DeliveryApp *app);
void destroyState(struct State *state);

void printMainMenu(struct State *state);
void printShopList(struct State *state);
void printOrderMenu(struct State *state);
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

struct State *createStateOrderMenu(struct DeliveryApp *app)
{
    struct State *state = (struct State *)malloc(sizeof(struct State));
    if (state == NULL)
    {
        return NULL;
    }

    state->app = app;
    state->printMenu = printOrderMenu;
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
    state->printMenu = NULL;
    state->bRunning = 0;
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

    scanf("%d", &input);
    switch (input)
    {
    case 1:
        changeState(app, createStateShopList(app));
        break;
    case 2:
        changeState(app, createStateOrderMenu(app));
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

void printOrderMenu(struct State *state)
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

    scanf("%d", &input);
    switch (input)
    {
    default:
        changeState(app, createStateMainMenu(app));
        break;
    }
}
