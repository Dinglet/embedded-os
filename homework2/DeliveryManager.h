#pragma once

#include "Shop.h"

struct DeliveryManager;
typedef struct DeliveryManager DeliveryManager;
typedef struct DeliveryManager *DeliveryManagerPtr;

DeliveryManagerPtr createDeliveryManager(int socket, ShopPtr shops[], int nShops);
void destroyDeliveryManager(DeliveryManagerPtr manager);

void runDeliveryManager(DeliveryManagerPtr manager);