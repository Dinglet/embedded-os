/**
 * game.c
 *
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/shm.h>

struct Data;
typedef struct Data Data;
typedef struct Data *DataPtr;

struct Data
{
    int guess;
    char result[8];
};

struct Game;
typedef struct Game Game;
typedef struct Game *GamePtr;

struct Game
{
    int target;
    int shmid;
    DataPtr pSharedData;
    int bRunning;
};

static GamePtr pGame = NULL;
static GamePtr createGame(int key, int guess);
static void executeGame(GamePtr pGame);
static void destroyGame(GamePtr pGame);

static void terminationHandler(int signum);
static void sigchldHandler(int signum);
static void guessHandler(int signo, siginfo_t *info, void *context);

// Usage: game <key> <guess>
int main(int argc, char const *argv[])
{
    int key, guess;

    if (argc != 3)
    {
        printf("Usage: %s <key> <guess>\n", argv[0]);
        return EXIT_FAILURE;
    }
    key = strtol(argv[1], NULL, 0); // not only for base 10
    guess = strtol(argv[2], NULL, 0);

    signal(SIGINT, terminationHandler);
    signal(SIGHUP, terminationHandler);
    signal(SIGCHLD, sigchldHandler);

    pGame = createGame(key, guess);
    if (pGame == NULL)
    {
        return EXIT_FAILURE;
    }
    executeGame(pGame);
    destroyGame(pGame);

    printf("[game] Terminated.\n");

    return 0;
}

GamePtr createGame(int key, int guess)
{
    struct sigaction signalAction;
    GamePtr pGame = (GamePtr)malloc(sizeof(Game));
    if (pGame == NULL)
    {
        perror("malloc");
        return NULL;
    }

    pGame->target = guess;

    // allocates a System V shared memory segment
    pGame->shmid = shmget(key, sizeof(Data), IPC_CREAT | 0660);
    if (pGame->shmid < 0)
    {
        perror("shmget");
        return NULL;
    }

    pGame->pSharedData = shmat(pGame->shmid, NULL, 0);
    if (pGame->pSharedData == (void *) -1)
    {
        perror("shmat");
        return NULL;
    }

    // register handler to SIGUSR1
    memset(&signalAction, 0, sizeof(signalAction));
    signalAction.sa_flags = SA_SIGINFO;
    signalAction.sa_sigaction = guessHandler;
    if (sigaction(SIGUSR1, &signalAction, NULL) < 0)
    {
        perror("sigaction");
        return NULL;
    }

    return pGame;
}

void executeGame(GamePtr pGame)
{
    printf("[game] Game PID: %d\n", getpid());

    pGame->bRunning = 1;
    while (pGame->bRunning)
    {
        pause();
    }
}

void destroyGame(GamePtr pGame)
{
    if (pGame->pSharedData != NULL)
    {
        shmdt(pGame->pSharedData);
        shmctl(pGame->shmid, IPC_RMID, NULL);
    }
    free(pGame);
}

void terminationHandler(int signum)
{
    pGame->bRunning = 0;
}

void sigchldHandler(int signum)
{
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void guessHandler(int signo, siginfo_t *info, void *context)
{
    int guess = pGame->pSharedData->guess;

    if (guess == pGame->target)
    {
        strncpy(pGame->pSharedData->result, "bingo", sizeof(pGame->pSharedData->result));
        // pGame->bRunning = 0;
    }
    else if (guess < pGame->target)
    {
        strncpy(pGame->pSharedData->result, "bigger", sizeof(pGame->pSharedData->result));
    }
    else
    {
        strncpy(pGame->pSharedData->result, "smaller", sizeof(pGame->pSharedData->result));
    }

    printf("[game] Guess: %d, %s\n", guess, pGame->pSharedData->result);

    kill(info->si_pid, SIGUSR1);
}
