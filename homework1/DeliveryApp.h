#pragma once

#include "Shop.h"

struct State;

struct DeliveryApp;

struct DeliveryApp *createDeliveryApp(struct Shop *shops[], int nShops);
void destroyDeliveryApp(struct DeliveryApp *app);

void runDeliveryApp(struct DeliveryApp *app);

int isRunning(struct DeliveryApp *app);
void printMenu(struct DeliveryApp *app);
void changeState(struct DeliveryApp *app, struct State *state);

void showNumber(int n);
void countDown(int n);
void *threadShowNumber(void *arg);
