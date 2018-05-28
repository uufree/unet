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
        
        RDMutexLock::RDMutexLock() :
            u_tid(-1)
        {
            u_mutex = (pthread_rwlock_t*)::malloc(sizeof(pthread_rwlock_t));
            if(u_mutex == NULL)
                unet::handleError(errno);
            if(::pthread_rwlock_init(u_mutex,NULL) < 0)
                unet::handleError(errno);
        }

        RDMutexLock::RDMutexLock(RDMutexLock&& lock) :
            u_tid(lock.u_tid),
            u_mutex(std::move(lock.u_mutex))
        {
            lock.u_mutex = NULL;
        };

        RDMutexLock& RDMutexLock::operator=(RDMutexLock&& lock)
        {
            if(*this == lock)
                return *this;
            u_tid = lock.u_tid;
            u_mutex = std::move(lock.u_mutex);
            lock.u_mutex = NULL;

            return *this;
        }

        RDMutexLock::~RDMutexLock()
        {
            if(u_mutex != NULL && u_tid == (unsigned long)-1)
            {
                if(::pthread_rwlock_destroy(u_mutex) < 0)
                    unet::handleError(errno);
                ::free(u_mutex);
            }
        }
        
        void RDMutexLock::rdLock()
        {
            if(u_mutex)
            {
                if(::pthread_rwlock_rdlock(u_mutex) < 0)
                    unet::handleError(errno);
                u_tid = unet::tid();
            }
            u_tid = -1;
        }
        
        void RDMutexLock::wrLock()
        {
            if(u_mutex)
            {
                if(::pthread_rwlock_wrlock(u_mutex) < 0)
                    unet::handleError(errno);
                u_tid = unet::tid();
            }
            u_tid = -1;
        }

        void RDMutexLock::unlock()
        {
            if(u_mutex)
            {
                if(::pthread_rwlock_unlock(u_mutex) < 0)
                    unet::handleError(errno);
                u_tid = -1;
            }
        }
    }
}

