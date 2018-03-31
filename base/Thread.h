/*************************************************************************
	> File Name: Thread.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年02月26日 星期日 00时35分15秒
 ************************************************************************/

#ifndef _THREAD_H
#define _THREAD_H

#include<pthread.h>
#include<functional>

#include"global.h"

namespace unet
{
    namespace base
    {
        class Thread final
        {
            typedef std::function<void()> ThreadFunc;
            static void* runInThread(void* arg);
            
            public:
                explicit Thread();
                explicit Thread(const ThreadFunc& lhs);
                Thread(const Thread& lhs) = delete;
                Thread(Thread&& lhs);
                Thread& operator=(const Thread& lhs) = delete;
                Thread& operator=(Thread&& lhs);
                ~Thread();
                
                bool operator==(const Thread& thread){return _threadId==thread._threadId;};
                void start();
                void join();
            
                bool isStart() const{return _isStart;};
                void setThreadCallBack(const ThreadFunc& cb){_threadFunc = cb;};
                pthread_t getThreadId() const{return _threadId;};  

            private:
                pthread_t _threadId;
                bool _isStart;
                ThreadFunc _threadFunc;
        };
    }
}
                        
#endif

