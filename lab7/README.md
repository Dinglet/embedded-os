# Lab 7

## Appendix A - Examples

### A.1 Signal

#### A.1.1 Catching Signals

The program [`sig_catch`](./examples/sig_catch.c) outputs the following messages if the command `kill -USR1 11146` is execute within 10 seconds after `sig_catch` is run.

```bash
Process (11146) is catching SIGUSR1 ...
Process (11034) sent SIGUSR1.
Done.
```

where `11146` is the process ID of `sig_catch`, and `11034` is of `kill`.

#### A.1.2 Atomic Access in Signal Handler

The program [`sig_count`](./examples/sig_count.c) outputs the following messages.

```bash
Process (3829) is catching SIGUSR1 ...
SIGUSR1 was raised <N> times
```

where `3829` is the process ID of `sig_count`, and `<N>` will be replace by the number of time the command `kill -USR1 3829` is execute whin 10 seconds after `sig_count` is run.

### A.2 Timer