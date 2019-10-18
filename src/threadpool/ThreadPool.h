#ifndef THREADPOOL_H_
#define THREADPOOL_H_

#include <functional>
#include <future>
#include <thread>
#include <mutex>
#include <queue>
#include <vector>
#include <functional>

#include "../taskfuture/TaskFuture.h"

namespace threadpool {
    class ThreadPool {
    private:
        using Task = std::function<void()>;

    private:
        bool    mShutDown;
        bool    mIsInit;
        const int   mMaxQueueSize;
        std::mutex                  mQueueLock;
        std::condition_variable     mQueueCV;
        std::vector<std::thread>    mThreadPool;
        std::queue<Task>            mTaskQueue;

    public:
        ThreadPool(const ThreadPool &) = delete;
        ThreadPool(ThreadPool &&) = delete;
        ThreadPool &    operator=(const ThreadPool &) = delete;
        ThreadPool &    operator=(ThreadPool &&) = delete;

    private:
        void    init();
        void    threadHandle();

    public:
        void    shutDown();
        void    flush();

        ThreadPool(const int );
        ~ThreadPool();

        template<typename _Callable, typename... _Args>
        auto    submit(_Callable && callable, _Args && ... args) -> Future<decltype(callable(args ...))> 
        {
            using ReturnType = decltype(callable(args...));
            std::function<ReturnType()> func = std::bind(std::forward<_Callable>(callable), std::forward<_Args>(args)...);

            auto task_ptr = std::make_shared<std::packaged_task<ReturnType()>> (func);

            Task threadFunc = [task_ptr]() {
                (*task_ptr)();
                return ;
            };
            
            Future<ReturnType>  submitResult;
            submitResult.mIsSubmit = true;
            submitResult.mTaskFuture = task_ptr->get_future();
            {
                std::unique_lock<std::mutex> lock(mQueueLock);
                if (mTaskQueue.size() == mMaxQueueSize) 
                {
                    submitResult.mIsSubmit = false;
                    return submitResult;
                }

                mTaskQueue.push(threadFunc);
            }
            mQueueCV.notify_all();

            return submitResult;


        }
    };
}

#endif
