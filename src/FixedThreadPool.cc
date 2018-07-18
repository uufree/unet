/*************************************************************************
	> File Name: FixedThreadPool.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年07月18日 星期三 20时26分53秒
 ************************************************************************/

#include"../include/FixedThreadPool.h"

namespace unet
{
    FixedThreadPool::FixedThreadPool(int numbers,int usleep,ThreadFunc& func) :
        u_threadSize(numbers),
        u_usleepTime(usleep),
        u_waitSleepTime((u_usleepTime>>4)),
        u_recordSleepTime(0),
        u_threadCallBack(func),
        u_threads(),
        u_exist(),
        u_mutex(),
        u_currentFinish(0)
    {
        if(numbers <= 0)
            numbers = DEFAULT_FIXEDTHREADPOOL_NUMBER;
        for(int i=0;i<u_threadSize;i++) {
            u_threads[i] = std::make_shared<base::Thread>(u_threadCallBack);
            u_exist[i] = std::make_pair(u_threads[i]->getThreadId(),true);
        }
    }

    FixedThreadPool::~FixedThreadPool()
    {
        for(int i=0;i<u_threadSize;i++) {
            if(u_exist[i].second) {
                u_threads[i]->stop();
                u_exist[i].second = false;
            }
        }
    }

    void FixedThreadPool::refill(int index)
    {
        if(index >= u_threadSize)
            return;
        if(u_exist[index].second)
            return;
        u_threads[index] = std::make_shared<base::Thread>(u_threadCallBack);
        u_exist[index] = std::make_pair(u_threads[index]->getThreadId(),true);
    }

    void FixedThreadPool::stop(int index)
    {
        if(index >= u_threadSize)
            return;
        if(!u_exist[index].second)
            return;
        u_threads[index]->stop();
        u_exist[index].second = false;
    }

    void FixedThreadPool::start()
    {
        for(int i=0;i<u_threadSize;i++) {
            if(!u_exist[i].second)
                refill(i);
            u_threads[i]->start();
        }
    }

    void FixedThreadPool::stop()
    {
        for(int i=0;i<u_threadSize;i++) {
            if(u_exist[i].second) {
                u_threads[i]->stop();
                u_exist[i].second = false;
            }
        }
    }
   
    /*取消超时线程，依赖于任务队列的完成情况*/
    void FixedThreadPool::update()
    {
    }

    int FixedThreadPool::wait()
    {
        while(u_recordSleepTime < u_usleepTime) {
            usleep(u_waitSleepTime);
            u_recordSleepTime += u_waitSleepTime;
            {
                base::MutexLockGuard guard(u_mutex);
                if(u_currentFinish == u_threadSize)
                    break;
            }
        }       
        
        if(u_recordSleepTime < u_usleepTime) {
            return 0;
        } else if(u_recordSleepTime >= u_usleepTime) {
            update();
            return -1; 
        }
        
        u_recordSleepTime = 0;
        {
            base::MutexLockGuard guard(u_mutex);
            u_currentFinish = 0;
        }
        return 0;
    }
}
