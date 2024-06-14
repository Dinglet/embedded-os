# Lab 6: Web ATM Using Inter-Process Communication

In lab 6, students learn the usage of the semaphore, mutex, pipe, and shared memory, and then apply knowledge to implement an ATM system.

## Web ATM

The system consists of two parts: a server application and a client application. The client sends deposit and withdraw requests to the server; the server application deals with the requests without a data race.

### Client

On my virtual machine, the kernel parameter `net.ipv4.ip_local_port_range` is set to `60700 61000`, which means that the number of TCP connections between my server and clients running on my VM cannot exceed 301 at once. The `connect` function in the `client` program will return `-1` with the error `EADDRNOTAVAIL 99 Cannot assign requested address` when there are too many local sockets open at once. Therefore, I open only one socket for 1 client to send transaction data to the server as a workaround.

### Server

The server establishes a socket connection on the port specified by the first argument. Whenever it accepts a new connection request, it starts a new thread to receive data related to a series of transactions. The critical section involves updating and printing the account balance, and it is protected by a System V semaphore.
