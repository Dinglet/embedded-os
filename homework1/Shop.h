#pragma once
#include <stdlib.h>

struct MenuItem
{
    char *name;
    int price;
    struct MenuItem *next;
};

struct Restaurant
{
    char *name;
    int distance;
    struct MenuItem *menu;
};

struct Restaurant *createRestaurant(char *name, int distance);
void destroyRestaurant(struct Restaurant *restaurant);

void addMenuItem(struct Restaurant *restaurant, char *name, int price);
