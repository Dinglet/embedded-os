#pragma once

#include "Shop.h"

struct State;

struct DeliveryApp;
typedef struct DeliveryApp DeliveryApp;
typedef struct DeliveryApp *DeliveryAppPtr;

/// @brief Create a DeliveryApp object to handle a client shopping request.
DeliveryAppPtr createDeliveryApp(int clientSocket, struct Shop *shops[], int nShops);
void destroyDeliveryApp(DeliveryAppPtr app);

void runDeliveryApp(DeliveryAppPtr app);

int isRunning(DeliveryAppPtr app);
void printMenu(DeliveryAppPtr app);
void changeState(DeliveryAppPtr app, struct State *state);

void showNumber(int n);
void countDown(int n);
void *threadShowNumber(void *arg);
