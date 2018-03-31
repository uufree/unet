/*************************************************************************
	> File Name: ThreadPool.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月27日 星期一 23时19分53秒
 ************************************************************************/

#ifndef _THREADPOOL_H
#define _THREADPOOL_H

#include"base/Thread.h"
#include"base/Mutex.h"
#include"base/Condition.h"
#include"Channel.h"

#include<deque>
#include<memory>

//一个线程池只做一件事情

namespace unet
{
    class ThreadPool final
    {
        typedef std::function<void()> ThreadFunc;

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
                if(!_start)
                    _threadFunc = cb;
            }

            void setThreadCallBack(ThreadFunc&& cb)
            {
                if(!_start)
                    _threadFunc = std::move(cb);
            }

            void start();
            void joinAll();

        private:
            bool _start;
            const int _threadSize;
            base::Thread* _threadListPtr;
            ThreadFunc _threadFunc;
                
            base::MutexLock _mutex;
            base::Condition _cond;
    };
}
#endif

