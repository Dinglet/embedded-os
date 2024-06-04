#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#include "TaskConsumer.h"

static void *taskConsumerThread(void *arg)
{
    TaskConsumerPtr taskConsumer = (TaskConsumerPtr)arg;
    TaskList *taskList = taskConsumer->taskList;

    while (1)
    {
        sem_wait(&taskConsumer->signalTaskArrival);

        TaskPtr task = taskListFront(taskList);

        timespec_get(&taskConsumer->startTime, TIME_UTC);

        sleep(taskGetTime(task));

        sem_post(taskGetCompletionSignal(task));

        pthread_mutex_lock(&taskConsumer->mutex);
        taskListPopFront(taskList);
        pthread_mutex_unlock(&taskConsumer->mutex);
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
    pthread_mutex_init(&taskConsumer->mutex, NULL);

    pthread_create(&taskConsumer->thread, NULL, taskConsumerThread, taskConsumer);

    return taskConsumer;
}

void destroyTaskConsumer(TaskConsumerPtr taskConsumer)
{
    pthread_cancel(taskConsumer->thread);
    pthread_mutex_destroy(&taskConsumer->mutex);
    free(taskConsumer);
}

struct timespec taskConsumerGetStartTime(TaskConsumerPtr taskConsumer)
{
    if (taskListIsEmpty(taskConsumer->taskList))
    {
        struct timespec now;
        timespec_get(&now, TIME_UTC);
        return now;
    }

    return taskConsumer->startTime;
}

double taskConsumerEstimateRemainingTime(TaskConsumerPtr taskConsumer)
{
    int taskListTime;
    struct timespec startTime, currentTime;
    double elapsedTime;

    startTime = taskConsumerGetStartTime(taskConsumer);
    timespec_get(&currentTime, TIME_UTC);

    elapsedTime = difftime(currentTime.tv_sec, startTime.tv_sec) + 1e-9 * (currentTime.tv_nsec - startTime.tv_nsec);

    pthread_mutex_lock(&taskConsumer->mutex);
    taskListTime = taskListGetTime(taskConsumer->taskList);
    pthread_mutex_unlock(&taskConsumer->mutex);

    return taskListTime - elapsedTime;
}
