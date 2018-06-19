/*************************************************************************
	> File Name: SpinLock.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月29日 星期二 06时32分24秒
 ************************************************************************/

#ifndef _SPINLOCK_H
#define _SPINLOCK_H

#include<pthread.h>

/*实际效果证明：在短临界区的情况下，SpinLock的性能确实要优于MutexLock*/

namespace unet
{
    namespace base
    {
        /*为了应对多线程中对简短的标志的访问*/
        /*尽一切可能减少系统调用*/
        /*与Mutex的区别是：
         * Spin被持有时，其余线程原地等待，不会重新调度
         * Mutex被持有时，会发生重新调度
         */
        class SpinLock final
        {
            friend bool operator==(const SpinLock&,const SpinLock&);
            public:
                explicit SpinLock();
                SpinLock(const SpinLock&) = delete;
                SpinLock& operator=(const SpinLock&) = delete;
                SpinLock(SpinLock&&);
                SpinLock& operator=(SpinLock&&);
                ~SpinLock();
                
                void lock();
                void unlock();
                bool isLocked() const{return u_locked;};

            private:
                pthread_spinlock_t u_mutex;
                bool u_locked;
        };

        class SpinLockGuard final
        {   
            public:
                explicit SpinLockGuard(SpinLock& lock);
                SpinLockGuard(const SpinLockGuard&) = delete;
                SpinLockGuard(SpinLockGuard&&) = delete;
                SpinLockGuard& operator=(const SpinLockGuard&) = delete;
                SpinLockGuard& operator=(SpinLockGuard&&) = delete;
                ~SpinLockGuard();

            private:
                SpinLock& u_lock;
        };
    }
}

#endif
