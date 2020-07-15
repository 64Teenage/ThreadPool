#include "ThreadQueue.h"
#include "../threadlog/threadlog.h"
#include <cstdint>

void    ThreadQueue::update(std::shared_ptr<RuntimeJob> job, bool flag) {
    if(flag) {
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
}


void    ThreadQueue::push(std::shared_ptr<RuntimeJob> job, int prior) {
    
    DEG_LOG("start push... X %d", m_FreeList.size());
    m_FreeList.push(job, prior);
    DEG_LOG("end push... E %d", m_FreeList.size());

    job->updateDependency(m_DependencyMap);
    update(job, false);
    DEG_LOG("end push");
}

void    ThreadQueue::pop() {
    if (!access()) {
        throw ("no ready task");
    }

    DEG_LOG("pop job: %s", m_BusyList.front()->dump().c_str());

    m_BusyList.pop();
}

std::shared_ptr<RuntimeJob> ThreadQueue::front() {
    if (!access()) {
        throw ("no ready task");
    }

    return m_BusyList.front();
}

void    ThreadQueue::flush() {
    while(!m_BusyList.empty()) {
        m_BusyList.pop();
    }

    while(!m_FreeList.empty()) {
        m_FreeList.pop();
    }
}

bool    ThreadQueue::access() {
    return !m_BusyList.empty();
}

bool    ThreadQueue::empty() {
    return m_FreeList.empty() && m_BusyList.empty();
}

int     ThreadQueue::size() {
    return m_FreeList.size() + m_BusyList.size();
}
