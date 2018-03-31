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
        MutexLock::MutexLock() : _pid(0)
        {
            if(pthread_mutex_init(&_mutex,NULL) < 0)
                unet::handleError(errno);
        }

        MutexLock::~MutexLock()
        {
            if(pthread_mutex_destroy(&_mutex) < 0)
                unet::handleError(errno);
        }
        
        MutexLock::MutexLock(MutexLock&& lhs) : 
            _pid(std::move(lhs._pid)),
            _mutex(std::move(lhs._mutex))
        {}

        MutexLock& MutexLock::operator=(MutexLock&& lhs)
        {
            if(*this == lhs)
                return *this;
            _pid = std::move(lhs._pid);
            _mutex = std::move(lhs._mutex);
            return *this;
        }

        void MutexLock::lock()
        {        
            if(pthread_mutex_lock(&_mutex) < 0)
                    handleError(errno);
                _pid = pid();
        }

        void MutexLock::unlock()
        {
            if(pthread_mutex_unlock(&_mutex) < 0)
                handleError(errno);
            _pid = 0;
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

