/*************************************************************************
	> File Name: TaskPool.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年07月12日 星期三 20时52分01秒
 ************************************************************************/

#ifndef _TASKPOOL_H
#define _TASKPOOL_H

#include"ThreadPool.h"
#include"Mutex.h"
#include"Condition.h"
#include"Event.h"

#include<memory>
#include<vector>

namespace unet
{
    class TaskPool final
    {
        typedef std::function<void()> ThreadFunc;
        typedef std::shared_ptr<Event> EventPtr;
        typedef std::vector<EventPtr> EventPtrList;

        public:
            explicit TaskPool(int);
            TaskPool(const TaskPool&) = delete;
            TaskPool(TaskPool&&);
            TaskPool& operator=(const TaskPool&) = delete;
            TaskPool& operator=(TaskPool&&);
            ~TaskPool();
            
            bool operator==(const TaskPool& pool){return u_threadPool==pool.u_threadPool;};

            void start(){if(!u_start) u_threadPool.startAllThread();u_start=true;};
            void stop(){if(u_start) u_threadPool.stopAllThread();u_start=false;};
            void addInTaskEvent(EventPtrList& lhs);
            bool isStart() const{return u_start;};
            int taskSize() const{return u_eventList.size();};
            int threadSize() const{return u_threadSize;};

        private:
            void ThreadFunction();

        private:
            bool u_start;
            int u_threadSize;
            ThreadPool u_threadPool;    
            EventPtrList u_eventList;
                
            base::MutexLock u_mutex;
            base::Condition u_cond;
    };
}

#endif


