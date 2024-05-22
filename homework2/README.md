# Homework 2: Delivery Platform (online version)

## Introduction

In this homework, we will continue with the contents of [homework 1](../homework1/) and modify the original standalone delivery system to a connectable version by using a socket server.

## Specification

- All the deliveries are served by only one delivery person, and only one customer is served at a time.
    - When a customer connects to the system, the connection continues until the order is canceled or the order is delivered.
    - While one customer is connected, no other customer will be connected.
- The commands that can be execute by a customer are
    - `shop list`,
    - `order <item> <amount>`,
    - `confirm`, and
    - `cancel`
- To response to the `shop list` command, the serve should send the client the list of shops with meals.