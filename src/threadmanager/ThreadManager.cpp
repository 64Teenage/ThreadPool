#include <thread>
#include <mutex>
#include <iostream>
#include <condition_variable>

#include "ThreadManager.h"

ThreadManager::ThreadManager(int threads) : m_Running(true), m_Quit(false) {
    int maxHardThread = std::thread::hardware_concurrency();
    threads = (threads > maxHardThread ? maxHardThread : maxHardThread * 3 / 4);
    for (int pos = 0; pos < threads; ++pos) {
        m_ThreadCore.push_back(std::thread(&ThreadManager::ThreadWorkBody, this, pos));
    }
}

ThreadManager::~ThreadManager() {
    {
        std::lock_guard<std::mutex> lock(m_QueueLock);
        m_Running = false;
        m_Quit = true;
        m_QueueReady.notify_all();
    }

    for (auto & worker : m_ThreadCore) {
        if (worker.joinable()) {
            worker.join();
        }
    }

    flush();
}

void    ThreadManager::ThreadWorkBody(int threadID) {
    RuntimeJob jobHandle;
    while (m_Running) {
        {
            std::unique_lock<std::mutex> lock(m_QueueLock);
            m_QueueReady.wait(lock, [&](){return m_QueueTask.access() || m_Quit;});
            if (m_Quit) {
                return ;
            }
            jobHandle = m_QueueTask.front();
            m_QueueTask.pop();
        }

        jobHandle.run();

        onDependencyUpdate(jobHandle);
    }
}

void    ThreadManager::flush() {
    std::lock_guard<std::mutex> lock(m_QueueLock);
    m_QueueTask.flush();
}

void    ThreadManager::submit(const RuntimeJob & jobHandle, const int prior) {
    std::lock_guard<std::mutex> lock(m_QueueLock);
    m_QueueTask.push(jobHandle, prior);
    m_QueueReady.notify_all();
}

ThreadManager *  ThreadManager::getInstance() {
    static ThreadManager instance(32);
    return &instance;
}

void    ThreadManager::onDependencyUpdate( RuntimeJob & jobHandle) {
    std::lock_guard<std::mutex> lock(m_QueueLock);
    m_QueueTask.update(jobHandle, true);
    m_QueueReady.notify_all();
}