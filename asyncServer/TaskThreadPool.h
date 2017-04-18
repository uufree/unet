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

namespace unet
{
    namespace thread
    {
        class TaskThreadPool final
        {
            typedef std::function<void()> Task;
            typedef std::deque<Task> TaskQueue;
            typedef std::vector<Task> TaskList;
            typedef std::vector<unet::net::Channel*> ChannelList;
            typedef std::deque<unet::net::Channel*> ChannelQueue;
            
            public:
                TaskThreadPool(int size = 2) : pool1(size/2),pool2(size-size/2),lock()
                {
                    Thread thread(std::bind(&TaskThreadPool::ThreadFunc,this));
                    pool1.setThreadCallBack(std::move(thread));
                    Thread thread1(std::bind(&TaskThreadPool::ThreadFunc1,this));
                    pool2.setThreadCallBack(std::move(thread1));
                }

                TaskThreadPool(const TaskThreadPool& lhs) = delete;
                TaskThreadPool& operator=(const TaskThreadPool& lhs) = delete;
                ~TaskThreadPool()
                {
                    pool1.joinAll();
                    pool2.joinAll();
                }

                void start()
                {
                    pool1.start();
                    pool2.start();
                }
/*                
                void setThreadCallBack(const Thread& cb)
                {
                    pool.setThreadCallBack(cb);
                }
*/
                void addInTaskQueue(TaskList&& lhs)
                {
                    MutexLockGuard guard(lock);
                    queue.insert(queue.end(),lhs.begin(),lhs.end());
                }
                
                void addInChannelQueue(ChannelList* lhs)
                {
                    MutexLockGuard guard(lock);
                    channelqueue.insert(channelqueue.end(),lhs->begin(),lhs->end());
                    lhs->clear();
                } 

                int getTaskInPool(TaskQueue* lhs)
                {
                    MutexLockGuard guard(lock);
                    lhs->swap(queue);
                    return lhs->size();
                }

                int getChannelInPool(ChannelQueue* lhs)
                {
                    MutexLockGuard guard(lock);
                    lhs->swap(channelqueue);
                    return lhs->size();
                }

            private:

                void ThreadFunc()
                {
                    TaskQueue taskqueue;
                    Task task;
                    int queuesize = 0;
                    unet::net::Channel* channel_;

                    while(1)
                    {
                        queuesize = getTaskInPool(&taskqueue);
                        for(int i=0;i<queuesize;++i)
                        {
                            task = taskqueue.front();
                            taskqueue.pop_front();
                            task();
                        }
                        taskqueue.clear();

                        queuesize = getChannelInPool(&channelqueue);
                        for(int i=0;i<queuesize;++i)
                        {
                            channel_ = channelqueue.front();
                            channelqueue.pop_front();
                            channel_->handleEvent();
                        }
                        channelqueue.clear();
                    }
                }

                void ThreadFunc1()
                {
                    TaskQueue taskqueue;
                    Task task;
                    int queuesize = 0;
                    unet::net::Channel* channel_;

                    while(1)
                    {
                        queuesize = getChannelInPool(&channelqueue);
                        for(int i=0;i<queuesize;++i)
                        {
                            channel_ = channelqueue.front();
                            channelqueue.pop_front();
                            channel_->handleEvent();
                        }
                        channelqueue.clear();
                         
                        queuesize = getTaskInPool(&taskqueue);
                        for(int i=0;i<queuesize;++i)
                        {
                            task = taskqueue.front();
                            taskqueue.pop_front();
                            task();
                        }
                        taskqueue.clear();
                    }
                }



                ThreadPool pool1,pool2;
                TaskQueue queue;
                ChannelQueue channelqueue;
                MutexLock lock;
        };
    }
}
        

#endif

