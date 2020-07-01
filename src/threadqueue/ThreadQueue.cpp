#include "ThreadQueue.h"

void    ThreadQueue::update(RuntimeJob & job, bool flag) {
    if (flag) {
        job.onResultUpdate(m_DependencyMap);
    }
    if (m_SchedulReady) {
        dispatch(m_PrepareFreeList, m_PrepareBusyList, m_ReadyList/*, flag, job*/);
        m_SchedulReady = false;
    } else {
        dispatch(m_PrepareBusyList, m_PrepareFreeList, m_ReadyList/*, flag, job*/);
        m_SchedulReady = true;
    }
}

void    ThreadQueue::dispatch(PriorQueue<RuntimeJob> & from, PriorQueue<RuntimeJob> & to, PriorQueue<RuntimeJob> & worker/*, bool flag, RuntimeJob & job*/) {
    /*if (flag) {
        std::cout<<"finished: "<<job.getTaskID()<<std::endl;
    }
    */
    while(!from.empty()) {
        RuntimeJob job = from.front();
        from.pop();
        bool status = job.updateDependency(m_DependencyMap);
        unsigned int prior = job.getPriority();
        /*if (flag){
            job.dump();
        }
        */
        if (status) {
            worker.push(job, prior);
        } else {
            to.push(job, prior);
        }
    }

    fflush(stdout);
}

void    ThreadQueue::push(RuntimeJob job, int prior) {
    if (m_SchedulReady) {
        m_PrepareFreeList.push(job, prior);
    } else {
        m_PrepareBusyList.push(job, prior);
    }
    job.setDependency(m_DependencyMap);

    update(job, false);
}

void    ThreadQueue::pop() {
    if (!access()) {
        throw ("no ready task");
    }

    m_ReadyList.pop();
}

RuntimeJob &    ThreadQueue::front() {
    if (!access()) {
        throw ("no ready task");
    }

    return m_ReadyList.front();
}

void    ThreadQueue::flush() {
    while(!m_PrepareBusyList.empty()) {
        m_PrepareBusyList.pop();
    }

    while(!m_PrepareFreeList.empty()) {
        m_PrepareFreeList.pop();
    }

    while(!m_ReadyList.empty()) {
        m_ReadyList.pop();
    }
}

bool    ThreadQueue::access() {
    return !m_ReadyList.empty();
}

bool    ThreadQueue::empty() {
    return m_ReadyList.empty() && m_PrepareFreeList.empty() && m_PrepareBusyList.empty();
}

int     ThreadQueue::size() {
    return m_ReadyList.size() + m_PrepareFreeList.size() + m_PrepareBusyList.size();
}
