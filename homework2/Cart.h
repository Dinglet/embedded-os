#pragma once

#include "Shop.h"

struct Cart;
typedef struct Cart Cart;
typedef Cart* CartPtr;

CartPtr createCart(ShopPtr shop);
void destroyCart(CartPtr cart);

struct Cart
{
    ShopPtr shop;
    MenuItemPtr *items;
    int *quantities;
    int nItems;
};

void addItem(CartPtr cart, char *itemName, int quantity);
int getTotalPrice(CartPtr cart);
