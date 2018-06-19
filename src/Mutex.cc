/*************************************************************************
	> File Name: Mutex.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年06月13日 星期二 00时05分07秒
 ************************************************************************/

#include"../include/Mutex.h"

namespace unet
{
    namespace base
    {
        //MutexLock
        MutexLock::MutexLock() : 
            u_tid(0)
        {
            if(pthread_mutex_init(&u_mutex,NULL) < 0)
                unet::handleError(errno);
        }

        MutexLock::~MutexLock()
        {
            /*防止因为出现对象移动语义时，对对象的多次销毁*/
            if(pthread_mutex_destroy(&u_mutex) < 0)
                unet::handleError(errno);
        }
        
        MutexLock::MutexLock(MutexLock&& lhs) : 
            u_tid(lhs.u_tid),
            u_mutex(std::move(lhs.u_mutex))
        {
            lhs.u_tid = 0;
        }

        MutexLock& MutexLock::operator=(MutexLock&& lhs)
        {
            if(*this == lhs)
                return *this;
             
            u_tid = lhs.u_tid;
            u_mutex = std::move(lhs.u_mutex);
            lhs.u_tid = 0;
            return *this;
        }

        void MutexLock::lock()
        {  
            if(pthread_mutex_lock(&u_mutex) < 0)
                handleError(errno);
            u_tid = tid();
        }

        void MutexLock::unlock()
        {
            if(pthread_mutex_unlock(&u_mutex) < 0)
                handleError(errno);
            u_tid = 0;
        }
        
        //MutexLockGuard
        MutexLockGuard::MutexLockGuard(MutexLock& mutex_) :
            mutex(mutex_)
        {
            mutex.lock();
        }

        MutexLockGuard::~MutexLockGuard()
        {
            mutex.unlock();
        }
    }
}

