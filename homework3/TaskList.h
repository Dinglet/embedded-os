#pragma once

#include "Task.h"

struct TaskList;
typedef struct TaskList TaskList;
typedef struct TaskList *TaskListPtr;

TaskListPtr createTaskList();
void destroyTaskList(TaskListPtr taskList);
TaskListPtr createTaskLists(int nLists);
void destroyTaskLists(TaskListPtr taskLists, int nLists);

int taskListIsEmpty(TaskListPtr taskList);

void taskListAdd(TaskListPtr taskList, TaskPtr task);

int taskListGetTime(TaskListPtr taskList);
