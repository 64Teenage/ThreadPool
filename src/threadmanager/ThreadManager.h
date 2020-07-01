#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <chrono>
#include <unordered_map>
#include <list>

#include "../runtimejob/RuntimeJob.h"
#include "../threadqueue/ThreadQueue.h"

using namespace threadmanager;

struct Task {
    long    cnt;
    long    index;
    bool    modify;
    double  result;
};

class ThreadManager
{
public:

private:
    ThreadManager() = default; 
    ~ThreadManager();
    ThreadManager(int threads);

    ThreadManager(const ThreadManager &) = delete ;
    ThreadManager & operator=(const ThreadManager &) = delete ;

    void    ThreadWorkBody(int threadid);

    void    flush();
    void    submit(const RuntimeJob & jobHandle, const int prior);
    void    onDependencyUpdate(RuntimeJob & jobHandle);
    void    dispatch(PriorQueue<RuntimeJob> & from, PriorQueue<RuntimeJob> & to, PriorQueue<RuntimeJob> & worker);
    


public:
    static ThreadManager * getInstance();

    template<typename F, typename... Args>
    auto    PostJob(int taskid, int prior, std::list<int> dependency, F && f, Args &&... args) -> std::future<decltype(f(args...))>{
        using namespace threadmanager;
        RuntimeJob jobHandle(taskid, prior, dependency);
        using ReturnType = decltype(f(args...));
        std::function<ReturnType()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);

        auto task_ptr = std::make_shared<std::packaged_task<ReturnType()>>(func);
        std::function<void()> threadFunc = [task_ptr]() {
            (*task_ptr)();
            return;
        };

        jobHandle.setTaskHandle(threadFunc);

        submit(jobHandle, prior);

        auto task_future = task_ptr->get_future();
        return task_future;
    }

private:
    bool m_Running = false;
    bool m_Quit = false;
    std::mutex                  m_QueueLock;
    std::condition_variable     m_QueueReady;
    std::vector<std::thread>    m_ThreadCore;
    ThreadQueue                 m_QueueTask;
};
