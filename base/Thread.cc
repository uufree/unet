/*************************************************************************
	> File Name: Thread.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月27日 星期一 23时27分28秒
 ************************************************************************/

#include"Thread.h"
#include"Global.h"
#include<pthread.h>

namespace unet
{
    namespace base
    {    
        void* Thread::runInThread(void* arg)
        {
            ThreadFunc* thread(static_cast<ThreadFunc*>(arg));
            (*thread)();
            return 0;
        };

        Thread::Thread() : 
            u_threadId(-1),
            u_start(false)
        {};
        
        Thread::Thread(Thread&& lhs) : 
            u_threadId(-1),
            u_start(false),
            u_threadFunc(std::move(lhs.u_threadFunc))
        {};

        Thread& Thread::operator=(Thread&& lhs)
        {
            if(*this == lhs)
                return *this;

            u_threadId = -1;
            u_start = false;
            u_threadFunc = std::move(lhs.u_threadFunc);
            return *this;
        }

        Thread::~Thread()
        {
            if(u_start)
                stop();
        }
    
        void Thread::start()
        {
            if(!u_start)
                if(::pthread_create(&u_threadId,NULL,runInThread,&u_threadFunc)!=0)
                    unet::handleError(errno);
            u_start = true;
            ::pthread_detach(u_threadId);
        }

        void Thread::stop()
        {
            if(u_start)
            {
                ::pthread_cancel(u_threadId);
                u_start = false;
                u_threadId = -1;
            }
        }
    }
}

