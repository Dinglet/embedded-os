#pragma once

#include "Shop.h"

struct State
{
    struct DeliveryApp *app;
    void (*printMenu)(struct State *state);
    int bRunning;
};

struct DeliveryApp;

struct DeliveryApp *createDeliveryApp(struct Shop *shops[], int nShops);
void destroyDeliveryApp(struct DeliveryApp *app);

void runDeliveryApp(struct DeliveryApp *app);

int isRunning(struct DeliveryApp *app);
void printMenu(struct DeliveryApp *app);
void changeState(struct DeliveryApp *app, struct State *state);
