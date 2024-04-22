#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>

void handler(int signum)
{
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main(int argc, char *argv[])
{
    signal(SIGCHLD, handler);
}