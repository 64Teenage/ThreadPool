#ifndef TASK_QUEUE_H_
#define TASK_QUEUE_H_

#include <queue>
#include <vector>

#include "../taskfuture/TaskFuture.h"

using namespace threadpool;

const int MAXPRIORSIZE = 10;

class TaskQueue
{
private:
    int                 mQueueSize;
    std::vector<int>    mQueueCount{std::vector<int>(MAXPRIORSIZE, 0)};
    std::priority_queue<int>        mPriorQueue;
    std::vector<std::queue<Task>>   mTaskQueue;

private:
    TaskQueue(const TaskQueue &) = delete;
    TaskQueue & operator=(const TaskQueue &) = delete;

public:
    TaskQueue();
    //~TaskQueue();

    void    push(const Task &, int);
    Task &  front();
    void    pop();
    int     size();
    bool    empty();
    unsigned int    flush();

};

#endif