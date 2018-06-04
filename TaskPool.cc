/*************************************************************************
	> File Name: TaskPool.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年07月12日 星期三 20时55分39秒
 ************************************************************************/

#include"TaskPool.h"

namespace unet
{
    TaskPool::TaskPool(int size) :
        u_start(false),
        u_threadSize(size),
        u_threadPool(),
        u_eventList(),
        u_mutex(),
        u_cond(u_mutex)
    {
        u_threadPool.addThread(std::bind(&TaskPool::ThreadFunction,this),u_threadSize,"TASK THREAD");
    };
    
    TaskPool::TaskPool(TaskPool&& pool) :
       u_start(pool.u_start), 
       u_threadSize(pool.u_threadSize),
       u_threadPool(),
       u_eventList(),
       u_mutex(),
       u_cond(u_mutex)
    {
        {
            base::MutexLockGuard guard(u_mutex);
            std::swap(u_threadPool,pool.u_threadPool);
            std::swap(u_eventList,pool.u_eventList);
        }
    }
    
    TaskPool& TaskPool::operator=(TaskPool&& pool)
    {
        if(*this == pool)
            return *this;
        u_start = pool.u_start;
        u_threadSize = pool.u_threadSize;

        {
            base::MutexLockGuard guard(pool.u_mutex);
            {
                base::MutexLockGuard guard(u_mutex);
                std::swap(u_eventList,pool.u_eventList);
            }
        }
        
        return *this;
    }
    
    TaskPool::~TaskPool()
    {
        if(u_start)
            u_threadPool.stopAllThread();
    }

    void TaskPool::ThreadFunction()
    {
        EventPtrList eventList;
        while(1)
        {
            if(!u_start)
                continue;
            {
                base::MutexLockGuard guard(u_mutex);
                while(u_eventList.empty())
                    u_cond.wait();
                std::swap(eventList,u_eventList);
            }

            for(auto iter=eventList.begin();iter!=eventList.end();++iter)
                (*iter)->handleEvent();

            eventList.clear();
        }
    }

    void TaskPool::addInTaskEvent(EventPtrList& list)
    {
        base::MutexLockGuard guard(u_mutex);
        std::swap(list,u_eventList); 
        u_cond.notify();
    }
}
