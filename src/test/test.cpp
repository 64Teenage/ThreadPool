#include <iostream>
#include <functional>
#include <cstdlib>

#include "../threadpool/ThreadPool.h"

int func(int id) 
{
    int sum = 0;
    for (int pos = 0; pos <= id; ++pos) 
        sum += pos;
    
    return sum;
}

int main(int argc, char ** argv)
{
    if (argc != 3)
    {
        std::cerr<<"usage: test <pool> <queue>"<<std::endl;
        return 0;
    }

    int threadSize = std::atoi(argv[1]);
    int queueSize = std::atoi(argv[2]);

    threadpool::ThreadPool threadPool(threadSize);

    for (int pos = 0; pos < queueSize; ++pos)
    {
        threadpool::Future<int> submitResult = threadPool.submit(func, pos);
        if (submitResult.mIsSubmit)
        {
            std::cout<<"Task "<<pos<<" submit successfully, the result is "<<submitResult.mTaskFuture.get()<<std::endl;
        }
        else
        {
            std::cout<<"Task "<<pos<<" submit failed!"<<std::endl;
        }
        
    }

    return 0;


}