#include <stdlib.h>
#include <unistd.h>
#include <limits.h>

#include "TaskList.h"
#include "TaskManager.h"

struct TaskManager
{
    TaskListPtr taskLists;
    int nLists;
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

    return taskManager;
}

void destroyTaskManager(TaskManagerPtr taskManager)
{
    destroyTaskLists(taskManager->taskLists, taskManager->nLists);
    free(taskManager);
}

int taskManagerEstimateWaitingTime(TaskManagerPtr taskManager, int executionTime)
{
    int estimatedTime = INT_MAX - executionTime;

    for (int i = 0; i < taskManager->nLists; i++)
    {
        #include "TaskList.h" // Include the header file that defines the taskListGetTime function

        int time = taskListGetTime(&taskManager->taskLists[i]);
        if (time < estimatedTime)
        {
            estimatedTime = time;
        }
    }

    return estimatedTime + executionTime;
}

void taskManagerAddTask(TaskManagerPtr taskManager, int executionTime, sem_t *pCompletionSignal)
{
    //TODO: Add task to the shortest list considering the estimated time

    sleep(executionTime);

    sem_post(pCompletionSignal);
}
