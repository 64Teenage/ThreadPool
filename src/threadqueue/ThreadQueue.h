#ifndef _THREADQUEUE_H_
#define _THREADQUEUE_H_

#include <unordered_map>

#include "../priorqueue/PriorQueue.h"
#include "../runtimejob/RuntimeJob.h"

using namespace threadmanager;

class ThreadQueue
{
public:
    ThreadQueue() {}
    ~ThreadQueue() {}

    void    update(RuntimeJob & job, bool flag);

    void    push(RuntimeJob job, int prior);
    void    pop();
    bool    empty();
    bool    access();
    int     size();
    void    flush();

    RuntimeJob &    front();


private:
    void    dispatch(PriorQueue<RuntimeJob> & from, PriorQueue<RuntimeJob> & to, PriorQueue<RuntimeJob> & worker/*, bool flag, RuntimeJob & job*/);

private:
    bool                            m_SchedulReady;
    PriorQueue<RuntimeJob>          m_ReadyList;
    PriorQueue<RuntimeJob>          m_PrepareFreeList;
    PriorQueue<RuntimeJob>          m_PrepareBusyList;
    std::unordered_map<int,bool>    m_DependencyMap;
};


#endif
