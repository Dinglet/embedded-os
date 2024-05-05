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
    free(cart->items);
    free(cart->quantities);

    free(cart);
}