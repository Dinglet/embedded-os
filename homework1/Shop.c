#include <string.h>

#include "Restaurant.h"

struct Restaurant *createRestaurant(char *name, int distance)
{
    struct Restaurant *restaurant;

    if (name == NULL || distance < 0)
    {
        return NULL;
    }

    restaurant = malloc(sizeof(struct Restaurant));
    if (restaurant == NULL)
    {
        return NULL;
    }

    restaurant->name = malloc(strlen(name) + 1);
    if (restaurant->name == NULL)
    {
        free(restaurant);
        return NULL;
    }

    strcpy(restaurant->name, name);
    restaurant->distance = distance;
    restaurant->menu = NULL;
    return restaurant;
}

void destroyRestaurant(struct Restaurant *restaurant)
{
    if (restaurant == NULL)
    {
        return;
    }

    free(restaurant->name);
    // free all menu items
    struct MenuItem *current = restaurant->menu;
    while (current != NULL)
    {
        struct MenuItem *next = current->next;
        free(current->name);
        free(current);
        current = next;
    }

    free(restaurant);
}

void addMenuItem(struct Restaurant *restaurant, char *name, int price)
{
    struct MenuItem *item;
    struct MenuItem *current;

    if (restaurant == NULL || name == NULL || price < 0)
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

    if (restaurant->menu == NULL)
    {
        restaurant->menu = item;
    }
    else
    {
        current = restaurant->menu;
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = item;
    }
}
