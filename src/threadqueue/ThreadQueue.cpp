#include "ThreadQueue.h"
#include "../threadlog/threadlog.h"
#include <cstdint>

void    ThreadQueue::update(std::shared_ptr<RuntimeJob> job, bool flag) {
    if(flag) {
        m_QueueLock.lock();
        job->onResultUpdate(m_DependencyMap);
    }
    
    DEG_LOG("update start... X %s", (flag ? "true" : "false"));
    DEG_LOG("update start FreeList size: %ld", m_FreeList.size());

    uint32_t total = static_cast<uint32_t>(m_FreeList.size());
    uint32_t cnt = 0;
    while(cnt++ < total) {
        auto task = m_FreeList.front();
        m_FreeList.pop();

        DEG_LOG("job infomation: %s", task->dump().c_str());

        auto prior = task->getPriority();
        auto status= task->updateDependency(m_DependencyMap);
        if (status) {
            m_BusyList.push(task, prior);
            DEG_LOG("job to busy list: %s", task->dump().c_str());
        } else {
            m_FreeList.push(task, prior);
            DEG_LOG("job to free list: %s", task->dump().c_str());
        }
    }
    DEG_LOG("update end FreeList size: %ld", m_FreeList.size());
    DEG_LOG("update end... E");

    m_QueueReady.notify_one();
    if(flag) {
        m_QueueLock.unlock();
    }
}


void    ThreadQueue::push(std::shared_ptr<RuntimeJob> job, int prior) {
    std::lock_guard<std::mutex> lock(m_QueueLock);
    DEG_LOG("start push... X %d", m_FreeList.size());
    m_FreeList.push(job, prior);
    DEG_LOG("end push... E %d", m_FreeList.size());

    job->updateDependency(m_DependencyMap);
    update(job, false);
    DEG_LOG("end push");
}


std::shared_ptr<RuntimeJob> ThreadQueue::wait_and_pop() {
    std::unique_lock<std::mutex> lock(m_QueueLock);
    m_QueueReady.wait(lock, [&](){return !m_BusyList.empty() || !m_ActiveStatus;});
    if(!m_ActiveStatus) {
        return nullptr;
    }

    auto job = m_BusyList.front();
    m_BusyList.pop();

    return job;
}

void    ThreadQueue::flush() {
    std::lock_guard<std::mutex> lock(m_QueueLock);
    while(!m_BusyList.empty()) {
        m_BusyList.pop();
    }

    while(!m_FreeList.empty()) {
        m_FreeList.pop();
    }
}

bool    ThreadQueue::access() {
    std::lock_guard<std::mutex> lock(m_QueueLock);
    return !m_BusyList.empty() || !m_ActiveStatus;
}

bool    ThreadQueue::access(bool active) {
    {
        std::lock_guard<std::mutex> lock(m_QueueLock);
        m_ActiveStatus = active;
        m_QueueReady.notify_all();
    }
    return active;
}

bool    ThreadQueue::empty() {
    std::lock_guard<std::mutex> lock(m_QueueLock);
    return m_FreeList.empty() && m_BusyList.empty();
}

int     ThreadQueue::size() {
    std::lock_guard<std::mutex> lock(m_QueueLock);
    return m_FreeList.size() + m_BusyList.size();
}
