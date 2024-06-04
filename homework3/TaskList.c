#include <stdlib.h>
#include <time.h>

#include "Task.h"
#include "TaskList.h"

struct Node
{
    TaskPtr task;
    NodePtr next;
};

// private functions
static void initTaskList(TaskListPtr taskList);
static void cleanTaskList(TaskListPtr taskList);

TaskListPtr createTaskList()
{
    TaskListPtr taskList = (TaskListPtr)malloc(sizeof(TaskList));
    if (taskList == NULL)
    {
        return NULL;
    }

    initTaskList(taskList);
    return taskList;
}

void destroyTaskList(TaskListPtr taskList)
{
    cleanTaskList(taskList);
    free(taskList);
}

TaskListPtr createTaskLists(int nLists)
{
    TaskListPtr taskLists = (TaskListPtr)malloc(nLists * sizeof(TaskList));
    if (taskLists == NULL)
    {
        return NULL;
    }

    for (int i = 0; i < nLists; i++)
    {
        initTaskList(&taskLists[i]);
    }

    return taskLists;
}

void destroyTaskLists(TaskListPtr taskLists, int nLists)
{
    for (int i = 0; i < nLists; i++)
    {
        cleanTaskList(&taskLists[i]);
    }
    free(taskLists);
}

int taskListIsEmpty(TaskListPtr taskList)
{
    return taskList->head == NULL;
}

void taskListAdd(TaskListPtr taskList, TaskPtr task)
{
    NodePtr node = (NodePtr)malloc(sizeof(Node));
    if (node == NULL)
    {
        return;
    }

    node->task = task;
    node->next = NULL;

    if (taskList->head == NULL)
    {
        taskList->head = node;
    }
    else
    {
        taskList->tail->next = node;
    }
    taskList->tail = node;
}

int taskListGetTime(TaskListPtr taskList)
{
    int time = 0;
    NodePtr node = taskList->head;
    while (node)
    {
        time += taskGetTime(node->task);
        node = node->next;
    }
    return time;
}

// private functions

void initTaskList(TaskListPtr taskList)
{
    taskList->head = NULL;
    taskList->tail = NULL;
}

void cleanTaskList(TaskListPtr taskList)
{
    NodePtr node = taskList->head;
    while (node)
    {
        NodePtr next = node->next;
        free(node);
        node = next;
    }
    taskList->head = NULL;
    taskList->tail = NULL;
}
