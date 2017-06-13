/*************************************************************************
	> File Name: Mutex.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年06月13日 星期二 00时05分07秒
 ************************************************************************/

#include"Mutex.h"
#include<utility>

namespace unet
{
    namespace thread
    {
        MutexLock::MutexLock() : pid(0)
        {
            if(pthread_mutex_init(&mutex,NULL) < 0)
                unet::handleError(errno);
        }

        MutexLock::~MutexLock()
        {
            if(pthread_mutex_destroy(&mutex) < 0)
                unet::handleError(errno);
        }
        
        MutexLock::MutexLock(MutexLock&& lhs) : 
            pid(std::move(lhs.pid)),
            mutex(std::move(lhs.mutex))
        {}

        MutexLock& MutexLock::operator=(MutexLock&& lhs)
        {
            pid = std::move(lhs.pid);
            mutex = std::move(lhs.mutex);

            return *this;
        }

        bool MutexLock::isLockInThisThread() const
        {
            return pid == now::pid();
        }
        
        void MutexLock::lock()
        {}

        void MutexLock::unlock()
        {}

        pthread_mutex_t& MutexLock::getMutex()
        {}
        
        
        

    }
}

