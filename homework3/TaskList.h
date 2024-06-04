#pragma once

#include "Task.h"

// linked list node
struct Node;
typedef struct Node Node;
typedef struct Node *NodePtr;

struct TaskList;
typedef struct TaskList TaskList;
typedef struct TaskList *TaskListPtr;

// linked list
struct TaskList
{
    NodePtr head;
    NodePtr tail;
};

TaskListPtr createTaskList();
void destroyTaskList(TaskListPtr taskList);
TaskListPtr createTaskLists(int nLists);
void destroyTaskLists(TaskListPtr taskLists, int nLists);

int taskListIsEmpty(TaskListPtr taskList);

void taskListAdd(TaskListPtr taskList, TaskPtr task);

TaskPtr taskListFront(TaskListPtr taskList);

void taskListPopFront(TaskListPtr taskList);

int taskListGetTime(TaskListPtr taskList);
