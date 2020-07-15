#include <iostream>
#include <fstream>
#include <queue>
#include <chrono>
#include <string>
#include <unordered_set>
#include <string>
#include <future>
#include <queue>
#include <list>

#include <unistd.h>

#include "../priorqueue/PriorQueue.h"
#include "../threadmanager/ThreadManager.h"

double  DoWork(long cnt) {
    PriorQueue<long> rank;
    std::string log = std::to_string(static_cast<long>(pthread_self())) + ": "+ std::to_string(cnt);
    std::cout<< log <<std::endl;
    auto start = std::chrono::system_clock::now();
    for (long pos = 0; pos < cnt; ++pos) {
        rank.push(pos, static_cast<int>(pos %10));
    }
    auto stop = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    auto result = static_cast<double>(duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den;
    return result;
}

int     Sum(int lhs, int rhs) {
    std::cout<< lhs + rhs <<std::endl;
    return lhs+rhs;
}

double  DefaultQueue(long cnt);



int main(int argc, char ** argv)
{

    if (argc != 2) {
        std::cerr<<"need one param"<<std::endl;
        return -1;
    }

    long cnt = atol(argv[1]);

    ThreadManager * threadPool = ThreadManager::getInstance();
    
    threadPool->PostJob(5, 5, std::list<int>{}, Sum, 1, 4);
    threadPool->PostJob(0, 5, std::list<int>{5,4}, Sum, 1, -1);
    threadPool->PostJob(1, 5, std::list<int>{4,3}, Sum, 1, 0);
    threadPool->PostJob(2, 5, std::list<int>{5}, Sum, 1, 1);
    threadPool->PostJob(3, 5, std::list<int>{2}, Sum, 1, 2);
    threadPool->PostJob(4, 9, std::list<int>{}, Sum, 1, 3);

    sleep(cnt);
    
    //std::cout << "Hello world" << std::endl;
    return 0;
}

