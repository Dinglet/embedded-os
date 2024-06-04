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

int taskManagerEstimateWaitingTime(TaskManagerPtr taskManager, int executionTime, size_t *pIndex)
{
    int estimatedTime = INT_MAX - executionTime;

    if (pIndex != NULL)
    {
        *pIndex = 0;
    }

    for (int i = 0; i < taskManager->nLists; i++)
    {
        #include "TaskList.h" // Include the header file that defines the taskListGetTime function

        int time = taskListGetTime(&taskManager->taskLists[i]);
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
    int estimatedTime = taskManagerEstimateWaitingTime(taskManager, executionTime, &index);

    taskListAdd(&taskManager->taskLists[index], &task);
    sem_post(&taskManager->taskConsumers[index]->signalTaskArrival);
}
