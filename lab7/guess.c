/**
 * guess.c
 *
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/shm.h>

struct Data;
typedef struct Data Data;
typedef struct Data *DataPtr;

struct Data
{
    int guess;
    char result[8];
};

struct Guesser;
typedef struct Guesser Guesser;
typedef struct Guesser *GuesserPtr;

struct Guesser
{
    int gamePid;
    int upper_bound;

    int shmid;
    DataPtr pSharedData;
    int bRunning;
};

GuesserPtr pGuess = NULL;
GuesserPtr createGuesser(int key, int upper_bound, int pid);
void executeGuesser(GuesserPtr pGuess);
void destroyGuesser(GuesserPtr pGuess);

void resultHandler(int signo, siginfo_t *info, void *context);

// guess <key> <upper_bound> <pid>
int main(int argc, char **argv)
{
    int key, upper_bound, pid;

    if (argc != 4)
    {
        printf("Usage: %s <key> <upper_bound> <pid>\n", argv[0]);
        return EXIT_FAILURE;
    }
    key = strtol(argv[1], NULL, 0);
    upper_bound = strtol(argv[2], NULL, 0);
    pid = strtol(argv[3], NULL, 0);

    pGuess = createGuesser(key, upper_bound, pid);
    if (pGuess == NULL)
    {
        return EXIT_FAILURE;
    }
    executeGuesser(pGuess);
    destroyGuesser(pGuess);

    return 0;
}

GuesserPtr createGuesser(int key, int upper_bound, int pid)
{
    GuesserPtr pGuess = (GuesserPtr)malloc(sizeof(Guesser));
    if (pGuess == NULL)
    {
        perror("malloc");
        return NULL;
    }

    pGuess->upper_bound = upper_bound;
    pGuess->gamePid = pid;

    pGuess->shmid = shmget(key, sizeof(Data), 0660);
    if (pGuess->shmid == -1)
    {
        perror("shmget");
        free(pGuess);
        return NULL;
    }

    pGuess->pSharedData = (DataPtr)shmat(pGuess->shmid, NULL, 0);
    if (pGuess->pSharedData == (DataPtr)-1)
    {
        perror("shmat");
        free(pGuess);
        return NULL;
    }

    struct sigaction sa;
    sa.sa_sigaction = resultHandler;
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &sa, NULL);

    return pGuess;

}

void executeGuesser(GuesserPtr pGuess)
{
    if (pGuess == NULL)
    {
        return;
    }

    // single guess
    pGuess->pSharedData->guess = pGuess->upper_bound / 2;
    printf("[game] Guess: %d\n", pGuess->pSharedData->guess);
    kill(pGuess->gamePid, SIGUSR1);
    pause();
}

void destroyGuesser(GuesserPtr pGuess)
{
    if (pGuess == NULL)
    {
        return;
    }

    shmdt(pGuess->pSharedData);
    free(pGuess);
}

void resultHandler(int signo, siginfo_t *info, void *context)
{
    if (info->si_pid != pGuess->gamePid)
    {
        // ignore
        return;
    }

    printf("Result: %s\n", pGuess->pSharedData->result);
}
