/*************************************************************************
	> File Name: Mutex.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年02月25日 星期六 23时04分44秒
 ************************************************************************/

#ifndef _MUTEX_H
#define _MUTEX_H

#include<pthread.h>
#include"CurrentThread.h"

//因为不是封装的库，所以在一些地方写的比较死，完全确认自己不会乱用

namespace unet
{
    namespace thread
    {
    
        class MutexLock final
        {
            public:
                explicit MutexLock() : pid(0)
                {
                    pthread_mutex_init(&mutex,NULL);
                };

                MutexLock(const MutexLock&) = delete;
                MutexLock& operator=(const MutexLock&) = delete;
                MutexLock(MutexLock&&) = delete;

                bool isLockInThisThread() const
                {
                    return pid == current::pid();
                };

                const pid_t getPid()
                {
                    return pid;
                }

                void lock()
                {
                    pthread_mutex_lock(&mutex);
                    pid = current::pid();
                };

                pthread_mutex_t* getMutex()
                {
                    return &mutex;
                };

                void unlock()
                {
                    pid = 0;
                    pthread_mutex_unlock(&mutex);
                };
            
                ~MutexLock()
                {
                    assert(pid == 0);
                    ::pthread_mutex_destroy(&mutex);
                }

            private:
                pid_t pid;
                pthread_mutex_t mutex;
        };
    
        class MutexLockGuard final
        {
            public:
                explicit MutexLockGuard(MutexLock& mutex_) : mutex(mutex_)
                {
                    mutex.lock();
                }

                ~MutexLockGuard()
                {
                    mutex.unlock();
                }
            
                MutexLockGuard(MutexLockGuard&) = delete;
                MutexLockGuard& operator=(const MutexLockGuard&) = delete;   
                MutexLockGuard(MutexLockGuard&&) = delete;

            private:
                MutexLock& mutex;
        };

        class Condition final
        {
            public:
                explicit Condition(MutexLock& mutex_) : mutex(mutex_)
                {
                    pthread_cond_init(&cond,NULL);
                }

                ~Condition()
                {
                    pthread_cond_destroy(&cond);
                }

                void notify()
                {
                    pthread_cond_signal(&cond);
                }

                void notifyAll()
                {
                    pthread_cond_broadcast(&cond);
                }

                void wait()
                {
                    assert(mutex.getPid() != 0);//mutex is locked
                    pthread_cond_wait(&cond,mutex.getMutex());    
                }

            private:
                MutexLock& mutex;
                pthread_cond_t cond;
        };
    }
}

#endif

