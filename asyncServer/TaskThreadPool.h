/*************************************************************************
	> File Name: TaskThreadPool.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年04月17日 星期一 18时06分48秒
 ************************************************************************/

#ifndef _TASKTHREADPOOL_H
#define _TASKTHREADPOOL_H

#include"../ThreadPool.h"
#include"../Mutex.h"
#include"../Condition.h"
#include"../Channel.h"
#include"../Condition.h"

namespace unet
{
    namespace thread
    {
        class TaskThreadPool final
        {
            typedef std::function<void()> Task;
            typedef std::vector<Task> TaskList;
            typedef std::vector<unet::net::Channel*> ChannelList;
            
            public:
                explicit TaskThreadPool(int size = 2) : pool(size),lock1(),lock2(),cond1(lock1),cond2(lock2)
                {
                    Thread thread(std::bind(&TaskThreadPool::ThreadFunc,this));
                    pool.setThreadCallBack(std::move(thread));
                }

                TaskThreadPool(const TaskThreadPool& lhs) = delete;
                TaskThreadPool& operator=(const TaskThreadPool& lhs) = delete;
                
                ~TaskThreadPool()
                {
                    pool.joinAll();
                }

                void start()
                {
                    pool.start();
                }

                void addInTaskQueue(TaskList&& lhs)
                {
                    MutexLockGuard guard(lock2);
                    queue.insert(queue.end(),lhs.begin(),lhs.end());
                    cond2.notify();
                }

                void addInChannelQueue(ChannelList* lhs)//由server调用
                {
                    MutexLockGuard guard(lock1);
                    channelqueue.swap(*lhs);
                    cond1.notify();
                }

                int getChannelInPool(ChannelList* lhs)//由线程调用
                {
                    MutexLockGuard guard(lock1); 
                    while(channelqueue.empty())
                        cond1.wait();
                    
                    channelqueue.swap(*lhs);
                    return lhs->size();
                }

                int getTaskInPool(TaskList* lhs)
                {
                    MutexLockGuard guard(lock2);
                    while(queue.empty())
                        cond2.wait();

                    lhs->swap(queue);
                    return lhs->size();
                }

            private:

                void ThreadFunc()
                {
                    TaskList taskqueue;
                    ChannelList channels;
                    Task task;
                    int queuesize = 0;

                    while(1)
                    { 
/*                        
                        taskqueue.clear();
                        queuesize = getTaskInPool(&taskqueue);
                        for(int i=0;i<queuesize;++i)
                        {
                            task = taskqueue.back();
                            taskqueue.pop_back();
                            task();
                        }
*/                    
                        channels.clear();
                        queuesize = getChannelInPool(&channels);
                        for(int i=0;i<queuesize;++i)
                            channels[i]->handleEvent();
                    }
                }

                ThreadPool pool;
                TaskList queue;
                ChannelList channelqueue;
                MutexLock lock1,lock2;
                Condition cond1,cond2;
        };
    }
}
        

#endif

