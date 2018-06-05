/*************************************************************************
	> File Name: Condition.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年07月17日 星期一 19时35分29秒
 ************************************************************************/

#include"Condition.h"

/*不忍直视，太简陋了*/
namespace unet
{
    namespace base
    {
        Condition::Condition(MutexLock& mutex) :
            u_mutex(mutex)
        {
            if(pthread_cond_init(&u_cond,NULL) < 0)
                unet::handleError(errno);
        }

        Condition::~Condition()
        {
            if(pthread_cond_destroy(&u_cond) < 0)
                unet::handleError(errno);
        }
        
        void Condition::notify()
        {
            if(pthread_cond_signal(&u_cond) < 0)
                unet::handleError(errno);
        }

        void Condition::notifyAll()
        {
            if(pthread_cond_broadcast(&u_cond) < 0)
                unet::handleError(errno);
        }

        void Condition::wait()
        {
            if(pthread_cond_wait(&u_cond,&u_mutex.getMutex()) < 0)
                unet::handleError(errno);
        }
        
        bool operator==(const Condition& lhs,const Condition& rhs)
        {return lhs.u_mutex == rhs.u_mutex;};
    }
}

