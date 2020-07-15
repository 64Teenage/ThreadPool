#ifndef _THREADQUEUE_H_
#define _THREADQUEUE_H_

#include <unordered_map>
#include <memory>

#include "../priorqueue/PriorQueue.h"
#include "../runtimejob/RuntimeJob.h"

using namespace threadmanager;

class ThreadQueue
{
public:
    ThreadQueue() {}
    ~ThreadQueue() {}

    void    push(RuntimeJob job, int prior);
    void    pop();
    bool    empty();
    bool    access();
    int     size();
    void    flush();

    std::shared_ptr<RuntimeJob>    front();

    void    update(std::shared_ptr<RuntimeJob>, bool);
    void    push(std::shared_ptr<RuntimeJob>, int);



private:

    PriorQueue<std::shared_ptr<RuntimeJob>>     m_BusyList;
    PriorQueue<std::shared_ptr<RuntimeJob>>     m_FreeList;
    std::unordered_map<int,bool>    m_DependencyMap;
};


#endif
