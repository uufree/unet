/*************************************************************************
	> File Name: FixedThreadPool.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年07月18日 星期三 19时58分49秒
 ************************************************************************/

#ifndef _FIXEDTHREADPOOL_H
#define _FIXEDTHREADPOOL_H

#include"Mutex.h"
#include"Thread.h"

#include<memory>
#include<array>
#include<vector>

namespace unet
{
    static int DEFAULT_FIXEDTHREADPOOL_NUMBER = 4;
    class FixedThreadPool final
    {
        typedef std::function<void()> ThreadFunc;
        typedef std::shared_ptr<base::Thread> ThreadPtr;
        
        public:
            FixedThreadPool(int,int,ThreadFunc&);
            FixedThreadPool(const FixedThreadPool& lhs) = delete;
            FixedThreadPool(FixedThreadPool&&) = delete;
            FixedThreadPool& operator=(const FixedThreadPool& lhs) = delete;
            FixedThreadPool& operator=(FixedThreadPool&& lhs) = delete;
            ~FixedThreadPool();
            
            bool operator==(const FixedThreadPool& lhs){return u_exist[0].first==lhs.u_exist[0].first;};
            void start();
            void stop();
            int wait();

        private:
            void refill(int);
            void stop(int);
            void update();

        private:
            const int u_threadSize;
            long u_usleepTime;  //超时时间
            long u_waitSleepTime;   //wait时的唤醒时间
            long u_recordSleepTime;     //记录全部的睡眠时间
            ThreadFunc u_threadCallBack;;
            std::vector<ThreadPtr> u_threads;
            std::vector<std::pair<pthread_t,bool>> u_exist;
           
            base::MutexLock u_mutex;
            int u_currentFinish;
    };
}
#endif
