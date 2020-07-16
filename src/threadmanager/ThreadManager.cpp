#include <thread>
#include <mutex>
#include <iostream>
#include <condition_variable>

#include "ThreadManager.h"

ThreadManager::ThreadManager(int threads) {
    m_QueueTask.access(true);
    int maxHardThread = std::thread::hardware_concurrency();
    threads = (threads > maxHardThread ? maxHardThread : maxHardThread * 3 / 4);
    for (int pos = 0; pos < threads; ++pos) {
        m_ThreadCore.push_back(std::thread(&ThreadManager::ThreadWorkBody, this, pos));
    }
}

ThreadManager::~ThreadManager() {
    m_QueueTask.access(false);

    for (auto & worker : m_ThreadCore) {
        if (worker.joinable()) {
            worker.join();
        }
    }

    flush();
}

void    ThreadManager::ThreadWorkBody(int threadID) {
    std::shared_ptr<RuntimeJob> jobHandle = std::make_shared<RuntimeJob>();
    while (true) {
        jobHandle = m_QueueTask.wait_and_pop();
        if(!m_QueueTask.active()) {
            return ;
        }
        
        if(jobHandle == nullptr) {
            continue ;
        }


        jobHandle->run();

        onDependencyUpdate(jobHandle);
    }
}

void    ThreadManager::flush() {
    m_QueueTask.flush();
}

void    ThreadManager::submit(std::shared_ptr<RuntimeJob> jobHandle, const int prior) {
    m_QueueTask.push(jobHandle, prior);
}

ThreadManager *  ThreadManager::getInstance() {
    static ThreadManager instance(32);
    return &instance;
}

void    ThreadManager::onDependencyUpdate(std::shared_ptr<RuntimeJob> jobHandle) {
    m_QueueTask.update(jobHandle, true);
}
