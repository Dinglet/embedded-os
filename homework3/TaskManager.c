#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <semaphore.h>

#include "TaskList.h"
#include "TaskConsumer.h"
#include "TaskManager.h"

struct TaskManager
{
    TaskListPtr taskLists;
    int nLists;

    TaskConsumerPtr *taskConsumers;
};

TaskManagerPtr createTaskManager(int nLists)
{
    TaskManagerPtr taskManager = (TaskManagerPtr)malloc(sizeof(TaskManager));
    if (taskManager == NULL)
    {
        return NULL;
    }

    taskManager->taskLists = createTaskLists(nLists);
    taskManager->nLists = nLists;

    taskManager->taskConsumers = (TaskConsumerPtr *)malloc(nLists * sizeof(TaskConsumerPtr));
    if (taskManager->taskConsumers == NULL)
    {
        destroyTaskLists(taskManager->taskLists, nLists);
        free(taskManager);
        return NULL;
    }

    for (int i = 0; i < nLists; i++)
    {
        taskManager->taskConsumers[i] = createTaskConsumer(&taskManager->taskLists[i]);
    }

    return taskManager;
}

void destroyTaskManager(TaskManagerPtr taskManager)
{
    for (int i = 0; i < taskManager->nLists; i++)
    {
        destroyTaskConsumer(taskManager->taskConsumers[i]);
    }
    destroyTaskLists(taskManager->taskLists, taskManager->nLists);
    free(taskManager);
}

double taskManagerEstimateWaitingTime(TaskManagerPtr taskManager, int executionTime, size_t *pIndex)
{
    double estimatedTime = 0;

    if (taskManager->nLists == 0)
    {
        //ERROR: no task list
        return estimatedTime;
    }

    estimatedTime = taskConsumerEstimateRemainingTime(taskManager->taskConsumers[0]);
    if (pIndex != NULL)
    {
        *pIndex = 0;
    }

    for (int i = 1; i < taskManager->nLists; i++)
    {
        double time = taskConsumerEstimateRemainingTime(taskManager->taskConsumers[i]);
        if (time < estimatedTime)
        {
            estimatedTime = time;
            if (pIndex != NULL)
            {
                *pIndex = i;
            }
        }
    }

    return estimatedTime + executionTime;
}

void taskManagerAddTask(TaskManagerPtr taskManager, int executionTime, sem_t *pCompletionSignal)
{
    Task task;
    initTask(&task, executionTime, pCompletionSignal);

    size_t index;
    double estimatedTime = taskManagerEstimateWaitingTime(taskManager, executionTime, &index);

    pthread_mutex_lock(&taskManager->taskConsumers[index]->mutex);
    taskListAdd(&taskManager->taskLists[index], &task);
    pthread_mutex_unlock(&taskManager->taskConsumers[index]->mutex);

    sem_post(&taskManager->taskConsumers[index]->signalTaskArrival);
}
