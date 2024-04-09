#include <string.h>

#include "Shop.h"

struct Shop *createShop(char *name, int distance)
{
    struct Shop *shop;

    if (name == NULL || distance < 0)
    {
        return NULL;
    }

    shop = malloc(sizeof(struct Shop));
    if (shop == NULL)
    {
        return NULL;
    }

    shop->name = malloc(strlen(name) + 1);
    if (shop->name == NULL)
    {
        free(shop);
        return NULL;
    }

    strcpy(shop->name, name);
    shop->distance = distance;
    shop->menu = NULL;
    return shop;
}

void destroyShop(struct Shop *shop)
{
    if (shop == NULL)
    {
        return;
    }

    free(shop->name);
    // free all menu items
    struct MenuItem *current = shop->menu;
    while (current != NULL)
    {
        struct MenuItem *next = current->next;
        free(current->name);
        free(current);
        current = next;
    }

    free(shop);
}

void addMenuItem(struct Shop *shop, char *name, int price)
{
    struct MenuItem *item;
    struct MenuItem *current;

    if (shop == NULL || name == NULL || price < 0)
    {
        return;
    }

    item = malloc(sizeof(struct MenuItem));
    if (item == NULL)
    {
        return;
    }

    item->name = malloc(strlen(name) + 1);
    if (item->name == NULL)
    {
        free(item);
        return;
    }

    strcpy(item->name, name);
    item->price = price;
    item->next = NULL;

    if (shop->menu == NULL)
    {
        shop->menu = item;
    }
    else
    {
        current = shop->menu;
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = item;
    }
}
