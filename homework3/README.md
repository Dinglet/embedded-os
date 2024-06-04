# Homework 3: Delivery Platform (multi-user online version)

## Introduction

In this homework, we will continue with the contents of [homework 2](../homework2/) and modify the original single-user connectable version to a multi-user one.

## Specification

- All deliveries are served by two delivery persons and multiple customers are served at a time.
    - When a customer connects to the system, the connection continues until the order is canceled or the order is delivered.
    - While one customer is connected, another customer can be connected.
    - When multiple customers are connected at the same time, the server must be able to respond to the orders sent by these customers. However, due to the limited number of delivery persons, the delivery time may be longer.
    - The FCFS (first-come-first-served) strategy is used for delivery. According to the order received by the server, the order will be assigned to the delivery person who needs shorter waiting time.
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

    - server (when a delivery will more than 30 seconds)

    ```text
    ​​​​​​​​​​​​​​​​​​​​​​​​​​Your delivery will take a long time, do you want to wait?
    ```

    - client (confirming the long wait)

    ```text
    Yes
    ```

    - client (refusing the long wait and cancel the order)

    ```text
    No
    ```

    - server (after the client confirms the order)

    ```text
    Please wait a few minutes...
    # wait for <N> seconds for the shop with distance <N>
    ​​​​​​​​​​​​​​​​Delivery has arrived and you need to pay $<price>
    ```

- In response to the `cancel` command, the server should close the connection, since the client will not send any command to the server.

## Usage

- the server

    ```bash
    ./hw3 <port>
    ```

- `netcat` as the client

    ```bash
    nc localhost <port>
    ```
