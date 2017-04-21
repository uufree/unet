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
            typedef std::vector<Task> TaskQueue;
            typedef std::vector<Task> TaskList;
            typedef std::vector<unet::net::Channel*> ChannelList;
            typedef std::vector<unet::net::Channel*> ChannelQueue;
            
            public:
                TaskThreadPool(int size = 2) : pool(size),lock()
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
/*                
                void setThreadCallBack(const Thread& cb)
                {
                    pool.setThreadCallBack(cb);
                }
*/
/*                
                void addInTaskQueue(TaskList&& lhs)
                {
                    MutexLockGuard guard(lock);
                    queue.insert(queue.end(),lhs.begin(),lhs.end());
                }
*/                
                void addInChannelQueue(ChannelList* lhs)
                {
                    MutexLockGuard guard(lock);
                    channelqueue.swap(*lhs);
                    std::cout << "addInChannelQueue: " << (*channelqueue.begin())->getFd() << std::endl;
                    std::cout << "addInChannelQueue channelqueue.size: " << channelqueue.size() << std::endl;
                    lhs->clear();
                    sleep(1);
                }

                int getChannelInPool(ChannelList* lhs)
                {
                    MutexLockGuard guard(lock);
                    std::cout << "getChannelInPool channelqueue.size: " << channelqueue.size() << std::endl;
                    std::cout << "getChannelInPool lhs.size: " << lhs->size() << std::endl;
                    channelqueue.swap(*lhs);
                    channelqueue.clear();
                    if(lhs->size() != 0)
                        std::cout << "getChannelInPool: " << (*lhs->begin())->getFd() << std::endl;
                    return lhs->size();
                }
/*
                int getTaskInPool(TaskQueue* lhs)
                {
                    MutexLockGuard guard(lock);
                    lhs->swap(queue);
                    return lhs->size();
                }
*/
            private:

                void ThreadFunc()
                {
                    TaskQueue taskqueue;
                    ChannelQueue channels;
                    Task task;
                    int queuesize = 0;
                    unet::net::Channel* channel_;

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
                        taskqueue.clear();
*/                                         
/*                        
                        {
                            thread::MutexLockGuard guard(lock); 
                            queuesize = channelqueue.size();
                            while(queuesize == 0)
                                cond.wait();
                            
                            if(queuesize != 0)
                                channels.swap(channelqueue);

                            std::cout << "channelqueue.size: " << channelqueue.size() << std::endl;
                            std::cout << "channels.size: " << channels.size() << std::endl;
                        }
*/                      
                        queuesize = getChannelInPool(&channels);
                        std::cout << "queuesize: " << queuesize << std::endl;
                        for(int i=0;i<queuesize;++i)
                            std::cout << "channel[i]->getFd: " << channels[i]->getFd() << std::endl;
                        for(int i=0;i<queuesize;++i)
                        {
                            channel_ = channels[i];
                            printf("%ld-----\n",pthread_self());
                            channel_->handleEvent();
                            printf("%ld-----\n",pthread_self());
                        }
                        channels.clear();
                        channel_ = nullptr;
                    }
                }

                ThreadPool pool;
                TaskQueue queue;
                ChannelQueue channelqueue;
                MutexLock lock;
        };
    }
}
        

#endif

