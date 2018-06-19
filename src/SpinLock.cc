/*************************************************************************
	> File Name: SpinLock.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月29日 星期二 06时32分28秒
 ************************************************************************/

#include"../include/SpinLock.h"
#include"../include/Global.h"

/*2018.05.29 测试完成*/
namespace unet
{
    namespace base
    {
        //SpinLock
        bool operator==(const SpinLock& lhs,const SpinLock& rhs)
        {return lhs.u_mutex == rhs.u_mutex;}

        SpinLock::SpinLock() :
            u_locked(false)
        {
            if(::pthread_spin_init(&u_mutex,PTHREAD_PROCESS_PRIVATE) < 0)
                unet::handleError(errno);
        }

        SpinLock::SpinLock(SpinLock&& lock) :
            u_mutex(std::move(lock.u_mutex)),
            u_locked(false)
        {
            lock.u_locked = false;
        }

        SpinLock& SpinLock::operator=(SpinLock&& lock)
        {
            /*SpinLock没有绝对的特征值，省略判断*/
//            if(*this == lock)
//                return *this;
            
            u_mutex = std::move(lock.u_mutex);
            u_locked = false;
            lock.u_locked = false;
            return *this;
        }

        SpinLock::~SpinLock()
        {
            if(::pthread_spin_destroy(&u_mutex) < 0)
                unet::handleError(errno);
        }

        void SpinLock::lock()
        {
            if(::pthread_spin_lock(&u_mutex) < 0)
                unet::handleError(errno);
            u_locked = true;
        }
        
        void SpinLock::unlock()
        {
            if(::pthread_spin_unlock(&u_mutex) < 0)
                unet::handleError(errno);
            u_locked = false;
        }
        
        //SpinLockGuard
        SpinLockGuard::SpinLockGuard(SpinLock& lock) :
            u_lock(lock)
        {u_lock.lock();};

        SpinLockGuard::~SpinLockGuard()
        {u_lock.unlock();};
    }
}

