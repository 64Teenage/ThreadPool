#ifndef THREADPOOL_H_
#define THREADPOOL_H_

#include <functional>
#include <future>
#include <thread>
#include <mutex>
#include <queue>
#include <vector>


class   ThreadPool;

class   ThreadWorker
{
private:
    int             m_tid;
    ThreadPool *    m_threadPool;

public:
    ThreadWorker(ThreadPool *   pool, const int id);
    void    operator() ();
};

class   ThreadPool
{
public:
    using Task = std::function<void()>;
    friend  class ThreadWorker;

private:
    bool        m_shutDown;
    std::mutex  m_mutex;
    std::condition_variable     m_condition;
    std::vector<std::thread>    m_threads;
    std::queue<Task>            m_taskQueue;

public:
    ThreadPool(const ThreadPool &) = delete;
    ThreadPool(ThreadPool &&) = delete;
    ThreadPool &    operator=(const ThreadPool &) = delete;
    ThreadPool &    operator=(ThreadPool &&) = delete;

public:
    void    init();
    void    shutDown();

    ThreadPool(const int );
    ~ThreadPool();

    template <typename F, typename ...Args>
    auto    submit(F && f, Args && ... args) -> std::future<decltype(f(args...))>
    {
        std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);

        auto    task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>> (func);

        Task threadFunc = [task_ptr]()
        {
            (*task_ptr)();
        };

        m_mutex.lock();
        m_taskQueue.push(threadFunc);
        m_mutex.unlock();

        m_condition.notify_all();

        return  task_ptr->get_future();
    }
};

#endif
