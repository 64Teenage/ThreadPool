#include <functional>
#include <future>
#include <thread>
#include <mutex>
#include <queue>
#include <vector>


#include "ThreadPool.h"

threadpool::ThreadPool::~ThreadPool()
{
    shutDown();
}

threadpool::ThreadPool::ThreadPool(const int queueSize) : mShutDown(false), mIsInit(false), mMaxQueueSize(queueSize)
{
    init();
}

void    threadpool::ThreadPool::init()
{
    if (!mIsInit)
    {
        for (unsigned int pos = 0; pos < mMaxQueueSize; ++pos)
        {
            mThreadPool.push_back(std::thread(&ThreadPool::threadHandle, this));
        }

        mIsInit = true;
    }
}

void    threadpool::ThreadPool::threadHandle()
{
    Task threadFunc = NULL;
    while (!mShutDown) 
    {
        {
            std::unique_lock<std::mutex> lock(mQueueLock);
            if (mTaskQueue.empty())
                mQueueCV.wait(lock);

            if (mTaskQueue.empty())
                continue;

            threadFunc = mTaskQueue.front();
            mTaskQueue.pop();
        }
        threadFunc();
    }
}

void    threadpool::ThreadPool::shutDown()
{
    if (mShutDown)
        return ;

    mShutDown = true;
    mQueueCV.notify_all();

    for (auto & threads : mThreadPool)
        if (threads.joinable())
            threads.join();
}

void    threadpool::ThreadPool::flush()
{
    std::queue<Task> emptyQueue;
    std::lock_guard<std::mutex> lock(mQueueLock);
    mTaskQueue.swap(emptyQueue);

}
