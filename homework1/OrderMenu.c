#include <stdio.h>
#include <stdlib.h>

#include "OrderMenu.h"

OrderMenuPtr createOrderMenu(ShopPtr shop)
{
    OrderMenuPtr orderMenu = (OrderMenuPtr)malloc(sizeof(OrderMenu));
    if (orderMenu == NULL)
    {
        return NULL;
    }

    orderMenu->shop = shop;
    orderMenu->cart = createCart(shop);

    return orderMenu;
}

void destroyOrderMenu(OrderMenuPtr orderMenu)
{
    destroyCart(orderMenu->cart);

    free(orderMenu);
}

int showOrderMenu(OrderMenuPtr orderMenu)
{
    int input, quantity, ret;
    while (1)
    {
        printf("Please choose from 1~%d\n", orderMenu->cart->nItems + 2);
        for (int i = 0; i < orderMenu->cart->nItems; i++)
        {
            printf("%d. %s: %d\n", i + 1, orderMenu->cart->items[i]->name, orderMenu->cart->items[i]->price);
        }
        printf("%d. confirm\n", orderMenu->cart->nItems + 1);
        printf("%d. cancel\n", orderMenu->cart->nItems + 2);

        scanf(" %d", &input);
        if (input < 1 || input > orderMenu->cart->nItems + 2)
        {
            printf("Invalid input\n");
            continue;
        }
        if (input == orderMenu->cart->nItems + 1)
        {
            return STATUS_CONFIRM;
        }
        if (input == orderMenu->cart->nItems + 2)
        {
            return STATUS_CANCEL;
        }

        do
        {
            printf("How many?\n");
            scanf(" %d", &quantity);
            ret = addItemToOrder(orderMenu, input - 1, quantity);
            if (ret == -1)
            {
                printf("Invalid input\n");
            }
        } while (ret == -1);
    }
    return 0;
}

int addItemToOrder(OrderMenuPtr orderMenu, int menuItemIndex, int quantity)
{
    if (orderMenu == NULL)
        return -1;
    if (menuItemIndex < 0 || menuItemIndex >= orderMenu->cart->nItems)
        return -1;
    if (quantity < -orderMenu->cart->quantities[menuItemIndex])
        return -1;

    orderMenu->cart->quantities[menuItemIndex] += quantity;
    return 0;
}

int getTotalPrice(OrderMenuPtr orderMenu)
{
    CartPtr cart = orderMenu->cart;
    int totalPrice = 0;
    for (int i = 0; i < cart->nItems; i++)
    {
        totalPrice += cart->quantities[i] * cart->items[i]->price;
    }

    return totalPrice;
}
