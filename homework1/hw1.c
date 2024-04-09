#include <stdio.h>

#include "Restaurant.h"

void printRestaurant(struct Restaurant *restaurant);

int main(int argc, char const *argv[])
{
    struct Restaurant *restaurants[3];
    restaurants[0] = createRestaurant("Dessert shop", 3);
    addMenuItem(restaurants[0], "cookie", 60);
    addMenuItem(restaurants[0], "cake", 80);
    restaurants[1] = createRestaurant("Beverage shop", 5);
    addMenuItem(restaurants[1], "tea", 40);
    addMenuItem(restaurants[1], "boba", 70);
    restaurants[2] = createRestaurant("Diner", 8);
    addMenuItem(restaurants[2], "fried rice", 120);
    addMenuItem(restaurants[2], "egg-drop soup", 50);

    printf("Restaurants:\n");
    for (int i = 0; i < 3; i++)
    {
        printRestaurant(restaurants[i]);
    }

    destroyRestaurant(restaurants[0]);
    destroyRestaurant(restaurants[1]);
    destroyRestaurant(restaurants[2]);
    return 0;
}

void printRestaurant(struct Restaurant *restaurant)
{
    if (restaurant == NULL)
    {
        return;
    }

    printf("Restaurant: %s\n", restaurant->name);
    printf("Distance: %d\n", restaurant->distance);
    printf("Menu:\n");
    struct MenuItem *current = restaurant->menu;
    while (current != NULL)
    {
        printf("%s: %d\n", current->name, current->price);
        current = current->next;
    }
}
