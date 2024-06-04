#pragma once

#include <pthread.h>
#include <semaphore.h>

#include "TaskList.h"

struct TaskConsumer;
typedef struct TaskConsumer TaskConsumer;
typedef struct TaskConsumer *TaskConsumerPtr;

struct TaskConsumer
{
    TaskListPtr taskList;

    pthread_t thread;
    sem_t signalTaskArrival;
};

TaskConsumerPtr createTaskConsumer(TaskListPtr taskList);
void destroyTaskConsumer(TaskConsumerPtr taskConsumer);


