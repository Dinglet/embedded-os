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
int getSocket(DeliveryAppPtr app);
