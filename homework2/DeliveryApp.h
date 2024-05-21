#pragma once

#include "Shop.h"

struct State;

struct DeliveryApp;
typedef struct DeliveryApp DeliveryApp;
typedef struct DeliveryApp *DeliveryAppPtr;

DeliveryAppPtr createDeliveryApp(struct Shop *shops[], int nShops, int clientSocket);
void destroyDeliveryApp(DeliveryAppPtr app);

void runDeliveryApp(DeliveryAppPtr app);

int isRunning(DeliveryAppPtr app);
void printMenu(DeliveryAppPtr app);
void changeState(DeliveryAppPtr app, struct State *state);

void showNumber(int n);
void countDown(int n);
void *threadShowNumber(void *arg);
