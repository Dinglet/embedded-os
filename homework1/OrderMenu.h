#pragma once

#include "Shop.h"
#include "Cart.h"

#define STATUS_CONFIRM 0
#define STATUS_CANCEL 1

struct OrderMenu;
typedef struct OrderMenu OrderMenu;
typedef OrderMenu* OrderMenuPtr;

struct OrderMenu
{
    struct Shop *shop;
    Cart *cart;
};

OrderMenuPtr createOrderMenu(ShopPtr shop);
void destroyOrderMenu(OrderMenuPtr orderMenu);

int showOrderMenu(OrderMenuPtr orderMenu);
int addItemToOrder(OrderMenuPtr orderMenu, int menuItemIndex, int quantity);
int getTotalPrice(OrderMenuPtr orderMenu);
