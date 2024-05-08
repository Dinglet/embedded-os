# Lab6: Inter-Process Communication

In Lab 6, students are asked to .... (TBA)

## Examples

### Semaphore

There are 2 types of semaphores in Linux, namely System V semaphores and POSIX semaphores[^semaphore-type]. In the directory, examples are example code to create/remove/use System V semantics.

[^semaphore-type]: Citation needed

- `makesem` creates a semaphore.
- `rmsem` removes a semaphore.
- `doodle` is an application with the wait (P) and signal (V) operations implemented, using a semaphore created previously.

### Race Condition

To compile the application `race` with the semaphore feature, run `make CFLAGS="-DUSE_SEM" race`. Otherwise, the command `make race` compiles with the macro `USE_SEM` undefined.

Before running `race`, run `echo 0 > counter.txt` to initialize a file.
