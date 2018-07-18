/*************************************************************************
	> File Name: Mutex.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年02月25日 星期六 23时04分44秒
 ************************************************************************/

#ifndef _MUTEX_H
#define _MUTEX_H

#include<pthread.h>
#include"Global.h"

/*对对象移动语义的测试：
 * 进行了对象的移动时，依旧会出现两个对象析构,需要对被移动对象的状态手动进行修改。
 * 栈上对象：可以认为在内部进行字节移动，不需要手动管理状态
 * 堆上对象：移动对象，指针置空
 */

/*2018.05.29 测试完成*/
namespace unet
{
    namespace base
    {   
        /*对线程锁这个资源使用RAII方法进行封装，即存在即初始化*/
        /*为了保证mutex可以与Cond配合使用，将mutex声明为mutable的*/
        class MutexLock final 
        {
            public:
                explicit MutexLock();
                MutexLock(const MutexLock&) = delete;
                MutexLock& operator=(const MutexLock&) = delete;
                MutexLock(MutexLock&& lhs);
                MutexLock& operator=(MutexLock&& lhs);
                ~MutexLock();
                    
                bool operator==(const MutexLock& lock) const
                {return u_tid==lock.u_tid;};
                
                bool isLockInThisThread()
                {return u_tid == tid();};
                
                pthread_mutex_t& getMutex()
                {return u_mutex;};
                
                void lock();
                void unlock();
            
            private:
                pthread_t u_tid;
                mutable pthread_mutex_t u_mutex;
        };
    
        class MutexLockGuard final
        {
            public:
                explicit MutexLockGuard(MutexLock& mutex);
                ~MutexLockGuard();
            
                MutexLockGuard(MutexLockGuard&) = delete;
                MutexLockGuard& operator=(const MutexLockGuard&) = delete;   
                MutexLockGuard(MutexLockGuard&&) = delete;
                MutexLockGuard& operator=(MutexLockGuard&&) = delete;
                void swap(MutexLockGuard& lhs) = delete;

            private:
                MutexLock& mutex;
        };
    }
}

#endif

