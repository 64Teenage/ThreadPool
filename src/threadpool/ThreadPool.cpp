#include <functional>
#include <future>
#include <thread>
#include <mutex>
#include <queue>
#include <vector>

#include <iostream>
#include <ctime>
#include <cstdio>

#include "ThreadPool.h"

using namespace threadpool;

threadpool::ThreadPool::~ThreadPool()
{
    shutDown();
}

threadpool::ThreadPool::ThreadPool(const int queueSize) : mShutDown(false), mIsInit(false), mMaxQueueSize(queueSize)
{
    int maxThreads = std::thread::hardware_concurrency();
    mThreadSize = queueSize < maxThreads ? queueSize : maxThreads;
    init();
}

void    threadpool::ThreadPool::init()
{
    if (!mIsInit)
    {
        for (unsigned int pos = 0; pos < mThreadSize; ++pos)
        {
            mThreadPool.push_back(std::thread(&ThreadPool::threadHandle, this));
        }

        mIsInit = true;
    }
}

void    threadpool::ThreadPool::threadHandle()
{
    Task threadFunc = NULL;
    while (true) 
    {
        {
            std::unique_lock<std::mutex> lock(mQueueLock);
            while (mTaskQueue.empty())
            {
                if (mShutDown)
                {
                    return;
                }
                mQueueCV.wait(lock);
            }

            threadFunc = mTaskQueue.front();
            mTaskQueue.pop();
        }
        threadFunc();
    }
}

void    threadpool::ThreadPool::shutDown()
{
    {
        std::lock_guard<std::mutex> lock(mQueueLock);
        mShutDown = true;
    }

    mQueueCV.notify_all();

    for (auto & threads : mThreadPool)
        if (threads.joinable())
            threads.join();
}

void    threadpool::ThreadPool::flush()
{
    std::lock_guard<std::mutex> lock(mQueueLock);
    mTaskQueue.flush();

}

bool    threadpool::ThreadPool::submit(const Task & task, int prior)
{
    std::unique_lock<std::mutex>    lock(mQueueLock);
    if (mTaskQueue.size() == mMaxQueueSize)
    {
        return false;
    }
    else
    {
        mTaskQueue.push(task, prior-1);
        mQueueCV.notify_all();
        return true;
    }
}
