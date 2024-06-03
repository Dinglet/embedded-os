#include <string.h>

#include "Cart.h"

CartPtr createCart(ShopPtr shop)
{
    CartPtr cart = (CartPtr)malloc(sizeof(Cart));
    if (cart == NULL)
    {
        return NULL;
    }

    cart->shop = shop;
    cart->nItems = countMenuItems(shop);
    cart->items = (MenuItemPtr *)malloc(sizeof(MenuItemPtr) * cart->nItems);
    cart->quantities = (int *)malloc(sizeof(int) * cart->nItems);

    MenuItemPtr current = shop->menu;
    for (int i = 0; i < cart->nItems; i++)
    {
        // Copy the menu items to the cart
        // ALERT: the cart does not own the menu items. It just references them. Don't free them before destroying the cart.
        cart->items[i] = current;
        cart->quantities[i] = 0;
        current = current->next;
    }

    return cart;
}

void destroyCart(CartPtr cart)
{
    if (cart == NULL)
    {
        return;
    }

    free(cart->items);
    free(cart->quantities);

    free(cart);
}

void addItem(CartPtr cart, char *itemName, int quantity)
{
    MenuItemPtr current = cart->shop->menu;
    for (int i = 0; i < cart->nItems; i++)
    {
        if (strcmp(current->name, itemName) == 0)
        {
            cart->quantities[i] += quantity;
            return;
        }
        current = current->next;
    }
}

int getTotalPrice(CartPtr cart)
{
    int totalPrice = 0;
    for (int i = 0; i < cart->nItems; i++)
    {
        totalPrice += cart->quantities[i] * cart->items[i]->price;
    }

    return totalPrice;
}
