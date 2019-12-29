#ifndef _TASKFUTURE_H_
#define _TASKFUTURE_H_

#include <future>
#include <functional>
#include <string>

#include <unistd.h>
#include <sys/time.h>

namespace threadpool {
    template<typename TypeName>
    struct Future {
        bool                    mIsSubmit;
        std::future<TypeName>   mTaskFuture;
    };

    using Task = std::function<void()>;

    struct TaskUnit
    {
    private:
        Task            mTaskHandle;
        pid_t           mSubmitThread;
        std::string     mSubmitTime;
        void            *mSourceHandle = nullptr;
    public:
        TaskUnit() = default;
        void    run();

    };
    
}

#endif