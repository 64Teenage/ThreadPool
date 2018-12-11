#include <functional>
#include <future>
#include <thread>
#include <mutex>
#include <queue>
#include <vector>


#include "ThreadPool.h"

ThreadWorker::ThreadWorker(ThreadPool * pool, const int id) : m_threadPool(pool), m_tid(id)
{}

ThreadPool::~ThreadPool()
{
    shutDown();
}

void        ThreadWorker::operator() ()
{
    ThreadPool::Task    threadFunc = NULL;

    while (!m_threadPool->m_shutDown)
    {
        std::unique_lock<std::mutex>    lock(m_threadPool->m_mutex);
        if (m_threadPool->m_taskQueue.empty())
            m_threadPool->m_condition.wait(lock);

        threadFunc = m_threadPool->m_taskQueue.front();
        m_threadPool->m_taskQueue.pop();
        

        threadFunc();
    }
}

ThreadPool::ThreadPool(const int iThreads) : m_threads(std::vector<std::thread>(iThreads)), m_shutDown(false)
{}

void        ThreadPool::init()
{
    for (std::vector<std::thread>::size_type pos=0; pos!=m_threads.size(); ++pos)
        m_threads[pos] = std::thread(ThreadWorker(this, pos));
}

void        ThreadPool::shutDown()
{
    if (m_shutDown)
        return ;

    m_shutDown = true;
    m_condition.notify_all();

    for (auto & threads : m_threads)
        if (threads.joinable())
            threads.join();
}
