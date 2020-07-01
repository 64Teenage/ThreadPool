#pragma once

#include <queue>
#include <vector>
#include <exception>

template<class T>
class PriorQueue
{
public:
    PriorQueue() : m_taskCount(std::vector<int>(10,0)), m_taskSize(0){
        for (int pos = 0; pos < 10; ++pos) {
            std::queue<T> rank;
            m_taskQueue.push_back(rank);
        }
    }
    ~PriorQueue() {}

    void    push(T value, int prior){
        if (prior < 0 || prior >= 10) {
            throw ("invalid prior");
        }

        bool active = m_taskCount.at(prior);
        if (active) {
        } else {
            m_priorkQueue.push(prior);
        }

        ++m_taskSize;
        ++m_taskCount.at(prior);
        m_taskQueue.at(prior).push(value);
    }

    void    pop() {
        if (empty()) {
            throw ("empty queue");
        }

        int  head = m_priorkQueue.top();
        
        --m_taskSize;
        --m_taskCount.at(head);
        m_taskQueue.at(head).pop();
        if (m_taskCount.at(head)) {
        } else {
            m_priorkQueue.pop();
        }
    }

    T &     front() {
        if (empty()) {
            throw ("empty queue");
        }

        int head = m_priorkQueue.top();

        return m_taskQueue.at(head).front();
    }

    bool    empty() {
        return m_taskSize == 0;
    }

    int     size() {
        return m_taskSize;
    }

    void    flush() {
        while (!empty()) {
            pop();
        }
    }



private:
    std::priority_queue<int>    m_priorkQueue;
    std::vector<int>            m_taskCount;
    std::vector<std::queue<T>>  m_taskQueue;

    int                         m_taskSize;

};

