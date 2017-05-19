/*************************************************************************
	> File Name: Condition.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月28日 星期二 16时04分46秒
 ************************************************************************/

#ifndef _CONDITION_H
#define _CONDITION_H

#include"Mutex.h"

namespace unet
{
    namespace thread
    { 
        class Condition final
        {
            public:
                explicit Condition(MutexLock& mutex_) : mutex(mutex_)
                {
                    if(pthread_cond_init(&cond,NULL) < 0)
                        unet::handleError(errno);
                }
            
                Condition(const Condition& lhs) = delete;
                Condition(Condition&& lhs) = delete;
                Condition& operator=(const Condition& lhs) = delete;
                Condition& operator=(Condition&& lhs) = delete;

                ~Condition()
                {
                    if(pthread_cond_destroy(&cond) < 0)
                        unet::handleError(errno);
                }

                void notify()
                {
                    if(pthread_cond_signal(&cond) < 0)
                        unet::handleError(errno);
                }

                void notifyAll()
                {
                    if(pthread_cond_broadcast(&cond) < 0)
                        unet::handleError(errno);
                }

                void wait()
                {
                    if(pthread_cond_wait(&cond,mutex.getMutex()) < 0)
                        unet::handleError(errno);
                }
    
            private:
                MutexLock& mutex;
                pthread_cond_t cond;
        };
    }
}

#endif

