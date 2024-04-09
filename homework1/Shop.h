#pragma once
#include <stdlib.h>

struct MenuItem
{
    char *name;
    int price;
    struct MenuItem *next;
};

struct Shop
{
    char *name;
    int distance;
    struct MenuItem *menu;
};

struct Shop *createShop(char *name, int distance);
void destroyShop(struct Shop *shop);

void addMenuItem(struct Shop *shop, char *name, int price);
