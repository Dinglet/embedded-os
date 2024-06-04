#include <stdlib.h>
#include <time.h>
#include <semaphore.h>

#include "Task.h"

void initTask(TaskPtr task, int time, sem_t *pCompletionSignal)
{
    task->executionTime = time;
    task->pCompletionSignal = pCompletionSignal;
}

void cleanTask(TaskPtr task)
{
    task->executionTime = 0;
    task->pCompletionSignal = NULL;
}

TaskPtr createTask(int time, sem_t *pCompletionSignal)
{
    TaskPtr task = (TaskPtr)malloc(sizeof(Task));
    if (task == NULL)
    {
        return NULL;
    }

    initTask(task, time, pCompletionSignal);

    return task;
}

void destroyTask(TaskPtr task)
{
    cleanTask(task);
    free(task);
}

int taskGetTime(TaskPtr task)
{
    return task->executionTime;
}

sem_t *taskGetCompletionSignal(TaskPtr task)
{
    return task->pCompletionSignal;
}
