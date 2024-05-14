# Lab 6: Web ATM Using Inter-Process Communication

In lab 6, students learn the usage of the semaphore, mutex, pipe, and shared memory, and then apply knowledge to implement an ATM system.

## Web ATM

The system consists of two parts: a server application and a client application. The client sends deposit and withdraw requests to the server; the server application deals with the requests without a data race.

### Client

On my virtual machine, the kernel parameter `net.ipv4.ip_local_port_range` is set to `60700 61000`, which means that the number of TCP connections between my server and clients running on my VM cannot exceed 301 at once. The `connect` function in the `client` program will return `-1` with the error `EADDRNOTAVAIL 99 Cannot assign requested address` when there are too many local sockets open at once. Therefore, I open only one socket for 1 client to send transaction data to the server as a workaround.

### Server

The server establishes a socket connection on the port specified by the first argument. Whenever it accepts a new connection request, it starts a new thread to receive data related to a series of transactions. The critical section involves updating and printing the account balance, and it is protected like the programs [`examples/doodle`](./examples/doodle.c) and [`examples/race`](./examples/race.c) do.

## Appendix A - Examples

### A.1 Semaphore

There are 2 types of semaphores in Linux, namely System V semaphores and POSIX semaphores[^semaphore-type]. In the directory, examples are example code to create/remove/use System V semantics.

[^semaphore-type]: Citation needed

- `makesem` creates a semaphore.
- `rmsem` removes a semaphore.
- `doodle` is an application with the wait (P) and signal (V) operations implemented, using a semaphore created previously.

#### Race Condition

To compile the application `race` with the semaphore feature, run `make CFLAGS="-DUSE_SEM" race`. Otherwise, the command `make race` compiles with the macro `USE_SEM` undefined.

Before running `race`, run `echo 0 > counter.txt` to initialize a file.

### A.2 Mutex

In the mutex example application `mutex`, multiple threads are about to enter a critical section. A mutex shared with those threads protects the critical section.

### A.3 Pipe

The process `pipe <file>` opens the `<file>` for reading and then transfers contents to another process with a pipe. The receiver writes received contents to the standard output.

#### Select

### A.4 Shared Memory
