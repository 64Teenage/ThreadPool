#ifndef _TASKFUTURE_H_
#define _TASKFUTURE_H_

#include <future>

namespace threadpool {
    template<typename TypeName>
    struct Future {
        bool                    mIsSubmit;
        std::future<TypeName>   mTaskFuture;
    };
}

#endif