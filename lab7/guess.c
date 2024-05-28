/**
 * guess.c
 *
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/time.h>

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
    int lower_bound;
    int upper_bound;

    int shmid;
    DataPtr pSharedData;
    int bRunning;
};

static GuesserPtr pGuess = NULL;
static GuesserPtr createGuesser(int key, int upper_bound, int pid);
static void executeGuesser(GuesserPtr pGuess);
static void destroyGuesser(GuesserPtr pGuess);

static void timerHandler(int signum);
static void resultHandler(int signo, siginfo_t *info, void *context);

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

    pGuess->lower_bound = 1;
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
    struct sigaction sa;
    struct itimerval timer;

    /* Install timer_handler as the signal handler for SIGALRM */
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &timerHandler;
    sigaction(SIGALRM, &sa, NULL);

    /* Configure the timer to expire after 1 second */
    timer.it_value.tv_sec = 1;
    timer.it_value.tv_usec = 0;

    /* Reset the timer back to 1 second after expired */
    timer.it_interval.tv_sec = 1;
    timer.it_interval.tv_usec = 0;


    pGuess->bRunning = 1;
    /* Start a virtual timer */
    if (setitimer(ITIMER_REAL, &timer, NULL) < 0)
    {
        perror("setitimer");
        exit(EXIT_FAILURE);
    }

    do
    {
        pause();
    } while (pGuess->bRunning);
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

// make a guess
void timerHandler(int signum)
{
    pGuess->pSharedData->guess = (pGuess->lower_bound + pGuess->upper_bound) / 2;
    printf("[game] Guess: %d\n", pGuess->pSharedData->guess);
    kill(pGuess->gamePid, SIGUSR1);
}

void resultHandler(int signo, siginfo_t *info, void *context)
{
    if (info->si_pid != pGuess->gamePid)
    {
        // ignore
        return;
    }

    if (strcmp(pGuess->pSharedData->result, "bingo") == 0)
    {
        pGuess->bRunning = 0;
    }
    else if (strcmp(pGuess->pSharedData->result, "bigger") == 0)
    {
        pGuess->lower_bound = pGuess->pSharedData->guess + 1;
    }
    else if (strcmp(pGuess->pSharedData->result, "smaller") == 0)
    {
        pGuess->upper_bound = pGuess->pSharedData->guess - 1;
    }
    else
    {
        printf("Invalid result: %s\n", pGuess->pSharedData->result);
        exit(EXIT_FAILURE);
    }
}
