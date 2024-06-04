#pragma once

#include <time.h>

struct Task;
typedef struct Task Task;
typedef struct Task *TaskPtr;

TaskPtr createTask(int time);
void destroyTask(TaskPtr task);

int taskGetTime(TaskPtr task);
