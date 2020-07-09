# Thread Pool Cpp
A simple C++ 11 Thread Pool implementation   

## Feature  
1. An effective task dependence relation management  
2. A simple task priority schedule algorithm  
3. std::future to obtain asynchronous task results  

## Licensing  
Public Domain. If my licensing is wrong, please let me know. Use at your own risk for whatever you want. Feel free to change the namespaces as well. Apparently licensing is hard and complicated. If your country doesn't have a public domain, feel free to say you found this on the side of the road.  

## Usage  
```Cpp  
ThreadManager threadpool(4);
auto Compare = [](int left, int right){return left > right;}
auto task_future = threadpool.PostJob(1, 5, std::list<int>{}, Compare, 1, 2);
std::cout<< task_future->get() <<std::endl;
```  
when you want to post a job to thread pool, take ThreadManager::PostJob. Consider the following function prototype:  
```Cpp
/*
 *Input:
 *      taskid:         task id
 *      prior:          task prior(0~9)
 *      dependency:     task denpency,which contain dependent task id
 *      f               worker function
 *      args:           function params
 *Output:
 *      std::future<decltype(f(args...))>   future to get asynchronous taks results
*/
template<typename F, typename... Args>
 auto    PostJob(int taskid, int prior, std::list<int> dependency, F && f, Args &&... args) -> std::future<decltype(f(args...))>  
```

## How to compile  
download th source code, then use 'make' under the top level directory of this repo  