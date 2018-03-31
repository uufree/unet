/*************************************************************************
	> File Name: Condition.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年07月17日 星期一 19时35分29秒
 ************************************************************************/

#include"Condition.h"

namespace unet
{
    namespace base
    {
        Condition::Condition(MutexLock& mutex) :
            _mutex(mutex)
        {
            if(pthread_cond_init(&_cond,NULL) < 0)
                unet::handleError(errno);
        }

        Condition::Condition(Condition&& lhs) :
            _mutex(lhs._mutex),
            _cond(std::move(lhs._cond))
        {};
        
        Condition& Condition::operator=(Condition&& lhs)
        {
            _mutex = std::move(lhs._mutex);
            _cond = std::move(lhs._cond);
            
            return *this;
        }
        
        Condition::~Condition()
        {
            if(pthread_cond_destroy(&_cond) < 0)
                unet::handleError(errno);
        }
        
        void Condition::notify()
        {
            if(pthread_cond_signal(&_cond) < 0)
                unet::handleError(errno);
        }

        void Condition::notifyAll()
        {
            if(pthread_cond_broadcast(&_cond) < 0)
                unet::handleError(errno);
        }

        void Condition::wait()
        {
            if(pthread_cond_wait(&_cond,&_mutex.getMutex()) < 0)
                unet::handleError(errno);
        }
        

        
    }
}

