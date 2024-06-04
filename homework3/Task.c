#include <stdlib.h>
#include <time.h>
#include <semaphore.h>

#include "Task.h"

struct Task
{
    int executionTime;
    sem_t completionSignal;
};

TaskPtr createTask(int time)
{
    TaskPtr task = (TaskPtr)malloc(sizeof(Task));
    if (task == NULL)
    {
        return NULL;
    }

    task->executionTime = time;
    sem_init(&task->completionSignal, 0, 0);

    return task;
}

void destroyTask(TaskPtr task)
{
    free(task);
}

int taskGetTime(TaskPtr task)
{
    return task->executionTime;
}
