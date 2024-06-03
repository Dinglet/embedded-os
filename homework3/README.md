# Homework 2: Delivery Platform (online version)

## Introduction

In this homework, we will continue with the contents of [homework 1](../homework1/) and modify the original standalone delivery system to a connectable version using a socket server.

## Specification

- All deliveries are served by only one single delivery person, and one customer is served at a time.
    - When a customer connects to the system, the connection continues until the order is canceled or the order is delivered.
    - While one customer is connected, no other customer will be connected.
- The commands that a customer can execute are
    - `shop list`,
    - `order <item> <amount>`,
    - `confirm`, and
    - `cancel`
- In response to the `shop list` command, the serve should send the client the list of shops with meals. In this homework, we use a fixed list of shops with meals.
    - client

    ```text
    shop list
    ```

    - server

    ```text
    Dessert shop:3km
    - cookie:$60|cake:$80
    Beverage shop:5km
    - tea:$40|boba:$70
    Diner:8km
    - fried-rice:$120|Egg-drop-soup:$50
    ```

- In response to the `order` command, the server should take the order of the client. The client can only order from the same restaurant, whichever is the first order.
    - client

    ```text
    ​​​​​​​​​​​​​​​​order cake 1
    ```

    - server

    ```text
    cake 1
    ```

    - client

    ```text
    ​​​​​​​​​​​​​​​​order cookie 3
    ```

    - server

    ```text
    ​​​​​​​​​​​​​​​​cookie 3|cake 1
    ```

    - client

    ```text
    order tea 1
    ```

    - server

    ```text
    ​​​​​​​​​​​​​​​​cookie 3|cake 1
    ```

- In response to the `confirm` command, the server should start the delivery if the client has ordered something. The client will disconnect from the server after the delivery arrives.
    - client

    ```text
    confirm
    ```

    - server (when you confirm without ordering)

    ```text
    ​​​​​​​​​​​​​​​​​​​​​​​​​​​Please order some meals
    ```

    - server (when you have ordered something)

    ```text
    Please wait a few minutes...
    # wait for <N> seconds for the shop with distance <N>
    ​​​​​​​​​​​​​​​​Delivery has arrived and you need to pay $<price>
    ```

- In response to the `cancel` command, the server should close the connection, since the client will not send any command to the server.

## Usage

- the server

    ```bash
    ./hw2 <port>
    ```

- `netcat` as the client

    ```bash
    nc localhost <port>
    ```
