# Lab 6: Web ATM Using Inter-Process Communication

In lab 6, students learn the usage of the semaphore, mutex, pipe, and shared memory, and then apply knowledge to implement an ATM application.

## Web ATM

The assignment of lab 6 requires a server application and a client application. The server application should deal with deposit and withdraw requests from multiple clients without a data race.

### Client

In the client application, the `connect` function will return `-1` with the error `EADDRNOTAVAIL` when there are too many local sockets open at once. Therefore, I open only one socket for 1 client to send transaction data to the server.

### Server

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
