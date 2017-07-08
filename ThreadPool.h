/*************************************************************************
	> File Name: ThreadPool.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月27日 星期一 23时19分53秒
 ************************************************************************/

#ifndef _THREADPOOL_H
#define _THREADPOOL_H

#include"Thread.h"
#include"Mutex.h"
#include"Condition.h"
#include<deque>
#include<memory>
#include"TaskQueue.h"
#include"Channel.h"

namespace unet
{
    namespace thread
    {
        class ThreadPool final
        {
            typedef std::function<void()> ThreadFunc;
            typedef std::vector<Channel&> ChannelList;

            public:
                explicit ThreadPool(int size = 2);
                explicit ThreadPool(int size,const ThreadFunc& cb);
                ThreadPool(const ThreadPool& lhs) = delete;
                ThreadPool(ThreadPool&& lhs);
                ThreadPool& operator=(const ThreadPool& lhs) = delete;
                ThreadPool& operator=(ThreadPool&& lhs);
                ~ThreadPool();

                void setThreadCallBack(const ThreadFunc& cb)
                {
                    if(!started)
                        threadFunc = cb;
                }

                void setThreadCallBack(ThreadFunc&& cb)
                {
                    if(!started)
                        threadFunc = std::move(cb);
                }

                void start();
                void joinAll();
                void addInTaskQueue(ChannelList& lhs);

            private:
                void ThreadFunction()
                {
                    ChannelList channels;
                    
                    while(1)
                    {
                        while((channelList.size()) == 0)   
                            cond.wait();
                        
                        channels.clear();
                        
                        {
                            MutexLockGuard guard(mutex);
                            std::swap(channels,channelList);    
                        }
                        
                        for(auto iter=channels.begin();iter!=channels.end();++iter)
                            iter->handleEvent();
                    }
                }

            private:
                bool started;
                const int threadSize;
                Thread* threadListPtr;
                ThreadFunc threadFunc;
                ChannelList channelList;

                MutexLock mutex;
                Condition cond;
        };
    }
}

#endif

