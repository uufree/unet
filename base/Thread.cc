/*************************************************************************
	> File Name: Thread.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月27日 星期一 23时27分28秒
 ************************************************************************/

#include"Thread.h"

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
            _threadId(0),
            _isStart(false)
        {};

        Thread::Thread(const ThreadFunc& lhs) : 
            _threadId(0),
            _isStart(false),
            _threadFunc(lhs)
        {};
       
        Thread::~Thread()
        {
            if(_isStart)
                if(_threadId)
                    ::pthread_detach(_threadId);;
        }
    
        Thread::Thread(Thread&& lhs) : 
            _threadId(0),
            _isStart(false),
            _threadFunc(lhs._threadFunc)
        {};

        Thread& Thread::operator=(Thread&& lhs)
        {
            if(*this == lhs)
                return *this;
            _threadId = 0;
            _isStart = false;
            _threadFunc = std::move(lhs._threadFunc);
            return *this;
        }

        void Thread::start()
        {
            if(!_isStart)
                if(::pthread_create(&_threadId,NULL,runInThread,&_threadFunc)!=0)
                    unet::handleError(errno);
            _isStart = true;
            _threadId = unet::pid();
        }

        void Thread::join()
        {
            if(_isStart)
            {
                ::pthread_join(_threadId,NULL);
                _isStart = false;
                _threadId = 0;
            }
        }
    }
}

