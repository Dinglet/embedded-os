#pragma once

#include "semaphore.h"

struct TaskManager;
typedef struct TaskManager TaskManager;
typedef struct TaskManager *TaskManagerPtr;

TaskManagerPtr createTaskManager(int nLists);
void destroyTaskManager(TaskManagerPtr taskManager);

int taskManagerEstimateWaitingTime(TaskManagerPtr taskManager, int executionTime);
void taskManagerAddTask(TaskManagerPtr taskManager, int executionTime, sem_t *pCompletionSignal);