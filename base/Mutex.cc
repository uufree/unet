/*************************************************************************
	> File Name: Mutex.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年06月13日 星期二 00时05分07秒
 ************************************************************************/

#include"Mutex.h"

namespace unet
{
    namespace base
    {
        MutexLock::MutexLock() : 
            u_tid(-1)
        {
            u_mutex = (pthread_mutex_t*)::malloc(sizeof(pthread_mutex_t));
            if(u_mutex == NULL)
                unet::handleError(errno);
            if(pthread_mutex_init(u_mutex,NULL) < 0)
                unet::handleError(errno);
        }

        MutexLock::~MutexLock()
        {
            /*防止因为出现对象移动语义时，对对象的多次销毁*/
            if(u_mutex != NULL && u_tid == (unsigned long)-1)
            {
                if(pthread_mutex_destroy(u_mutex) < 0)
                    unet::handleError(errno);
                ::free(u_mutex);
            }
        }
        
        MutexLock::MutexLock(MutexLock&& lhs) : 
            u_tid(lhs.u_tid),
            u_mutex(std::move(lhs.u_mutex))
        {
            lhs.u_mutex = NULL;
        }

        MutexLock& MutexLock::operator=(MutexLock&& lhs)
        {
            if(*this == lhs)
                return *this;
            
            u_tid = lhs.u_tid;
            u_mutex = std::move(lhs.u_mutex);
            lhs.u_mutex = NULL;
            return *this;
        }

        void MutexLock::lock()
        {  
            if(u_mutex)
            {
                if(pthread_mutex_lock(u_mutex) < 0)
                    handleError(errno);
                u_tid = tid();
            }
            u_tid = -1;
        }

        void MutexLock::unlock()
        {
            if(u_mutex)
            {
                if(pthread_mutex_unlock(u_mutex) < 0)
                    handleError(errno);
                u_tid = -1;
            }
        }

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

