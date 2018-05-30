/*************************************************************************
	> File Name: RDMutex.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月29日 星期二 05时30分52秒
 ************************************************************************/

#include"RDMutex.h"

namespace unet
{
    namespace base
    {
        bool operator==(const RDMutexLock& lhs,const RDMutexLock& rhs)
        {return lhs.u_tid == rhs.u_tid;};
        
        //RDMutexLock
        RDMutexLock::RDMutexLock() :
            u_tid(0)
        {
            if(::pthread_rwlock_init(&u_mutex,NULL) < 0)
                unet::handleError(errno);
        }

        RDMutexLock::RDMutexLock(RDMutexLock&& lock) :
            u_tid(lock.u_tid),
            u_mutex(std::move(lock.u_mutex))
        {
            lock.u_tid = 0;
        };

        RDMutexLock& RDMutexLock::operator=(RDMutexLock&& lock)
        {
            if(*this == lock)
                return *this;
            u_tid = lock.u_tid;
            u_mutex = std::move(lock.u_mutex);
            lock.u_tid = 0;
            return *this;
        }

        RDMutexLock::~RDMutexLock()
        {
            if(::pthread_rwlock_destroy(&u_mutex) < 0)
                unet::handleError(errno);
            u_tid = 0;
        }
        
        void RDMutexLock::rdLock()
        {
            if(::pthread_rwlock_rdlock(&u_mutex) < 0)
                unet::handleError(errno);
            u_tid = unet::tid();
        }
        
        void RDMutexLock::wrLock()
        {
            if(::pthread_rwlock_wrlock(&u_mutex) < 0)
                unet::handleError(errno);
            u_tid = unet::tid();
        }

        void RDMutexLock::unLock()
        {
            if(::pthread_rwlock_unlock(&u_mutex) < 0)
                unet::handleError(errno);
        }

        RDMutexLockGuard::RDMutexLockGuard(RDMutexLock& lock,enum LockType type) :
            u_lock(lock)
        {
            if(type == RDLOCK)
                u_lock.rdLock();
            else if(type == WRLOCK)
                u_lock.wrLock();
        }

        RDMutexLockGuard::~RDMutexLockGuard()
        {u_lock.unLock();};
    }
}

