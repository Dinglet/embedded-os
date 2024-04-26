# Lab 5

## Requirements

```bash
sudo apt install sl
```

## Demonstration

The `demo.sh` script runs the server program `lab5` and client programs `nc` (netcat) to display messages on the clients.

```bash
./lab5 <port>
```

After all the server and clients exit, we have to check if a zombie process exists by using the command:

```bash
ps axu | grep defunct | grep -v grep
```
