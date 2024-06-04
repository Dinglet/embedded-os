#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#include "TaskConsumer.h"

static void *taskConsumerThread(void *arg)
{
    TaskConsumerPtr taskConsumer = (TaskConsumerPtr)arg;
    TaskList *taskList = taskConsumer->taskList;

    while (1)
    {
        sem_wait(&taskConsumer->signalTaskArrival);

        TaskPtr task = taskListFront(taskList);

        sleep(taskGetTime(task));

        sem_post(taskGetCompletionSignal(task));

        taskListPopFront(taskList);
    }

    return NULL;
}

TaskConsumerPtr createTaskConsumer(TaskListPtr taskList)
{
    TaskConsumerPtr taskConsumer = (TaskConsumerPtr)malloc(sizeof(TaskConsumer));
    if (taskConsumer == NULL)
    {
        return NULL;
    }

    taskConsumer->taskList = taskList;

    pthread_create(&taskConsumer->thread, NULL, taskConsumerThread, taskConsumer);

    return taskConsumer;
}

void destroyTaskConsumer(TaskConsumerPtr taskConsumer)
{
    pthread_cancel(taskConsumer->thread);
    free(taskConsumer);
}
