#ifndef _THREADQUEUE_H_
#define _THREADQUEUE_H_

#include <unordered_map>
#include <memory>
#include <mutex>
#include <condition_variable>

#include "../priorqueue/PriorQueue.h"
#include "../runtimejob/RuntimeJob.h"

using namespace threadmanager;

class ThreadQueue
{
public:
    ThreadQueue(): m_ActiveStatus(false) {}
    ~ThreadQueue() {}

    void    push(RuntimeJob job, int prior);
    bool    empty();
    bool    access();
    int     size();
    void    flush();

    std::shared_ptr<RuntimeJob>    wait_and_pop();

    void    update(std::shared_ptr<RuntimeJob>, bool);
    void    push(std::shared_ptr<RuntimeJob>, int);

public:
    bool    access(bool);
    bool    active() {return m_ActiveStatus;}

private:

    bool    m_ActiveStatus;
    std::unordered_map<int,bool>    m_DependencyMap;
    PriorQueue<std::shared_ptr<RuntimeJob>>     m_BusyList;
    PriorQueue<std::shared_ptr<RuntimeJob>>     m_FreeList;

    std::mutex                  m_QueueLock;   
    std::condition_variable     m_QueueReady;
};


#endif
