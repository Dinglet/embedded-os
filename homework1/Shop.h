#pragma once
#include <stdlib.h>

struct Shop;
typedef struct Shop Shop;
typedef struct Shop *ShopPtr;

struct MenuItem;
typedef struct MenuItem MenuItem;
typedef struct MenuItem *MenuItemPtr;

struct MenuItem
{
    char *name;
    int price;
    MenuItemPtr next;
};

struct Shop
{
    char *name;
    int distance;
    MenuItemPtr menu;
};

ShopPtr createShop(char *name, int distance);
void destroyShop(ShopPtr shop);

void addMenuItem(ShopPtr shop, char *name, int price);
