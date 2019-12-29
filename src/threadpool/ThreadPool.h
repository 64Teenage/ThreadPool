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
#include "../funccast/funccast.h"
#include "../taskqueue/TaskQueue.h"

namespace threadpool {
    class ThreadPool {
    private:
        using Task = std::function<void()>;

    private:
        bool    mShutDown;
        bool    mIsInit;
        const int   mMaxQueueSize;
        int     mThreadSize;
        std::mutex                  mQueueLock;
        std::condition_variable     mQueueCV;
        std::vector<std::thread>    mThreadPool;
        TaskQueue                   mTaskQueue;

    public:
        ThreadPool(const ThreadPool &) = delete;
        ThreadPool(ThreadPool &&) = delete;
        ThreadPool &    operator=(const ThreadPool &) = delete;
        ThreadPool &    operator=(ThreadPool &&) = delete;

    private:
        void    init();
        void    threadHandle();

        template<typename _Callable, typename... _Args>
        bool    postJobAndSet(_Callable && callable, std::tuple<_Args...> params, int prior)
        {
            using namespace funccast;
            Cast<_Callable, _Args...>  cast(callable, params);
            auto func = cast.InvokePackage();

            bool result = submit(func, prior);
            return result;
        }

    public:
        void    shutDown();
        void    flush();
        bool    submit(const Task &, int);

        ThreadPool(const int );
        ~ThreadPool();

        

        template<typename _Callable, typename... _Args>
        bool    postJobAndSet(_Callable && callable, _Args && ... args)
        {
            using namespace funccast;
            ParamCast * instance = ParamCast::getInstance();
            auto funcParams = instance->remove_last(args...);
            int  prior = instance->getPriority();

            bool result = postJobAndSet(callable, funcParams, prior);

            return result;
        }

        template<typename _Callable, typename... _Args>
        auto    postJob(_Callable && callable, _Args && ... args) -> bool
        {
            using ReturnType = decltype(callable(args...));
            std::function<ReturnType()> func = std::bind(std::forward<_Callable>(callable), std::forward<_Args>(args)...);

            auto task_ptr = std::make_shared<std::packaged_task<ReturnType()>> (func);

            Task threadFunc = [task_ptr]() {
                (*task_ptr)();
                return ;
            };
            
            bool result = submit(threadFunc, 5);
            return result;


        }
    };
}

#endif
