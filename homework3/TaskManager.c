#include <stdlib.h>
#include <unistd.h>

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
    //TODO
    return 0;
}

void taskManagerAddTask(TaskManagerPtr taskManager, int executionTime, sem_t *pCompletionSignal)
{
    //TODO: Add task to the shortest list considering the estimated time

    sleep(executionTime);

    sem_post(pCompletionSignal);
}
