#ifndef _RUNTIMEJOB_H_
#define _RUNTIMEJOB_H_

#include <future>
#include <string>
#include <list>
#include <functional>
#include <thread>
#include <ctime>
#include <unordered_map>
#include <string>

namespace threadmanager {
    template<class T>
    struct Future {
        bool            submitResult;
        std::future<T>  mTaskFuture;
    };

    using Task = std::function<void()>;

    class RuntimeJob {
    private:
        unsigned int        m_TaskID;       //RuntimeJob的唯一标识符
        unsigned int        m_Prority;      //RuntimeJob的优先级
        std::list<int>      m_Dependency;   //该RuntimeJob的依赖
        std::thread::id     m_ThreadID;     //提交该任务的线程ID
        std::string         m_AddTime;      //任务提交时的时间
        Task                m_TaskHandle;   //任务函数

    public:
        RuntimeJob() = default;

        RuntimeJob(int taskID, int priority, std::list<int> dependency): m_TaskID(taskID), m_Prority(priority), m_Dependency(dependency) {
            char buffer[1024] = {0};
            std::time_t ctimeval = std::time(nullptr);
            m_AddTime = std::string(ctime_r(&ctimeval, buffer));
            m_ThreadID = std::this_thread::get_id();
        }

        void    setTaskHandle(const Task & task) {
            m_TaskHandle = task;
        }

        void    run() {
            m_TaskHandle();
        }

        unsigned int    getPriority() {
            return m_Prority;
        }

        unsigned int    getTaskID() {
            return m_TaskID;
        }

        bool    updateDependency(std::unordered_map<int,bool> & mapGraph) {
            for(auto it = m_Dependency.begin(); it != m_Dependency.end();) {
                if (mapGraph[*it] == true) {
                    it = m_Dependency.erase(it);
                } else {
                    ++it;
                }
            }
            return m_Dependency.empty();
        }

        void    setDependency(std::unordered_map<int,bool> & mapGraph) {
            if (mapGraph.count(m_TaskID) > 0) {
            } else {
                mapGraph[m_TaskID] = false;
            }

            for (auto it = m_Dependency.begin(); it != m_Dependency.end(); ++it) {
                if (mapGraph.count(*it) > 0) {
                } else {
                    mapGraph[*it] = false;
                }
            }
        }

        void    onResultUpdate(std::unordered_map<int,bool> & mapGraph) {
            mapGraph[m_TaskID] = true;
            //--mapGraph[m_TaskID];
        }

        std::string     dump() {
            std::string res;
            res += "Task id: " + std::to_string(m_TaskID);
            res += ", Priority: " + std::to_string(m_Prority);
            if(!m_Dependency.empty()) {
                res += ", Depend on:";
            }
            for (auto it = m_Dependency.begin(); it != m_Dependency.end(); ++it) {
                res += " " + std::to_string(*it);
            }
            return res;
        }
    };

}


#endif
