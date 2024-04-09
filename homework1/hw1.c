#include <stdio.h>

#include "Shop.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

void printShop(struct Shop *shop);

int main(int argc, char const *argv[])
{
    struct Shop *shops[3];
    shops[0] = createShop("Dessert shop", 3);
    addMenuItem(shops[0], "cookie", 60);
    addMenuItem(shops[0], "cake", 80);
    shops[1] = createShop("Beverage shop", 5);
    addMenuItem(shops[1], "tea", 40);
    addMenuItem(shops[1], "boba", 70);
    shops[2] = createShop("Diner", 8);
    addMenuItem(shops[2], "fried rice", 120);
    addMenuItem(shops[2], "egg-drop soup", 50);

    printf("Shops:\n");
    for (int i = 0; i < 3; i++)
    {
        printShop(shops[i]);
    }
    destroyShop(shops[0]);
    destroyShop(shops[1]);
    destroyShop(shops[2]);
    return 0;
}

void printShop(struct Shop *shop)
{
    if (shop == NULL)
    {
        return;
    }

    printf("Shop: %s\n", shop->name);
    printf("Distance: %d\n", shop->distance);
    printf("Menu:\n");
    struct MenuItem *current = shop->menu;
    while (current != NULL)
    {
        printf("%s: $%d\n", current->name, current->price);
        current = current->next;
    }
}
