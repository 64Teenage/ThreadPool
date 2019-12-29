#include <iostream>
#include <functional>
#include <cstdlib>
#include <cstdio>

#include "../threadpool/ThreadPool.h"

int func(int id) 
{
    int sum = 0;
    for (int pos = 0; pos <= id; ++pos) 
        sum += pos;
    
    std::cout<<id<<std::endl;
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
    
    /*
    for (int pos = 0; pos < queueSize; ++pos)
    {
        while ([&]() -> bool
        {
            bool submitResult = threadPool.postJob(func, pos);
            if (submitResult)
            {
                std::cout<<"Task "<<pos<<" submit success!"<<std::endl;
            }
            return !submitResult;
        }())
        std::cout<<"Task "<<pos<<" submit failed!"<<std::endl;
    }
    */

    for (int pos = 0; pos < queueSize; ++pos)
    {
        int result = threadPool.postJob(func, pos);
        if (result)
        {
            std::cout<<"Task "<<pos<<" submit success!"<<std::endl;
        }
        else
        {
            std::cout<<"Task "<<pos<<" submit failed!"<<std::endl;
        }
    }
    
    

    //threadPool.flush();

    return 0;


}