/*************************************************************************
	> File Name: BlockingQueue.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年02月27日 星期一 17时12分34秒
 ************************************************************************/

#ifndef _BLOCKINGQUEUE_H
#define _BLOCKINGQUEUE_H

#include<deque>
#include<assert.h>
#include"Mutex.h"
#include"Condition.h"

//多线程任务队列神器
namespace unet
{
    namespace thread
    {
        template<typename T>
        class BlockingQueue final
        {
            public:
                explicit BlockingQueue() : mutex(),notempty(mutex),queue() {};

                BlockingQueue(const BlockingQueue&) = delete;
                BlockingQueue(BlockingQueue&&) = delete;
                BlockingQueue& operator=(const BlockingQueue&) = delete;
                ~BlockingQueue() {};

                void put(const T& task)
                {
                    MutexLockGuard guard(mutex);
                    queue.push_back(T);
                    notempty.notify();
                }
//将C++11的move特性与旧版的区别开来            
#ifdef __GXX_EXPERIMENTAL_CXX0X__
                void put(T&& task)//调用者使用std::move
                {
                    MutexLockGuard guard(mutex);
                    queue.push_back(T);
                    notempty.notify();
                }
#endif                
            
                T get()
                {
                    MutexLockGuard guard(mutex);
                
                    while(queue.empty())
                    {
                        notempty.wait();
                    }
                    assert(!queue.empty());

#ifdef __GXX_EXPERIMENTAL_CXX0X__
                    T retu(std::move(queue.front()));
#else
                    T retu(queue.front());
#endif
                    queue.pop_front();
                    return retu;
                }

                size_t size()
                {
                    MutexLockGuard guard(mutex);
                    return queue.size();
                }

            private:
                std::deque<T> queue;
                mutable MutexLock mutex;
                Condition notempty;
        };
    }
}






#endif

