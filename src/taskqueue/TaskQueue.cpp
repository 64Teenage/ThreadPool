
#include "TaskQueue.h"

TaskQueue::TaskQueue() : mQueueSize(0)
{
    for (int pos = 0; pos < MAXPRIORSIZE; ++pos)
    {
        std::queue<Task>    task;
        mTaskQueue.push_back(task);
    }
}

void    TaskQueue::push(const Task & task, int prior)
{
    int head = mQueueCount[prior];
    if (head == 0)
    {
        mPriorQueue.push(prior);
    }
    ++mQueueCount[prior];
    mTaskQueue.at(prior).push(task);
    ++mQueueSize;
}

Task &  TaskQueue::front()
{
    int head = mPriorQueue.top();
    return mTaskQueue.at(head).front();
}

void    TaskQueue::pop()
{
    int head = mPriorQueue.top();
    --mQueueCount[head];
    if (mQueueCount[head] == 0)
    {
        mPriorQueue.pop();
    }
    mTaskQueue.at(head).pop();
    --mQueueSize;
}

int     TaskQueue::size()
{
    return mQueueSize;
}

bool    TaskQueue::empty()
{
    return mQueueSize == 0;
}

unsigned int    TaskQueue::flush()
{
    unsigned int taskSize = 0;
    while (!empty())
    {
        pop();
        ++taskSize;
    }

    return taskSize;
}