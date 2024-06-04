#pragma once

#include <time.h>
#include <semaphore.h>

struct Task;
typedef struct Task Task;
typedef struct Task *TaskPtr;

struct Task
{
    int executionTime;
    sem_t *pCompletionSignal;
};

void initTask(TaskPtr task, int time, sem_t *pCompletionSignal);
void cleanTask(TaskPtr task);

TaskPtr createTask(int time, sem_t *pCompletionSignal);
void destroyTask(TaskPtr task);

int taskGetTime(TaskPtr task);
sem_t *taskGetCompletionSignal(TaskPtr task);
