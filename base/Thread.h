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

namespace unet
{
    namespace base
    {
        /*
         * 封装Pthread的函数，对象的生命期决定线程的生命期
         */
        class Thread final
        {
            typedef std::function<void()> ThreadFunc;
            static void* runInThread(void* arg);
            
            public:
                explicit Thread();
                explicit Thread(const ThreadFunc&);
                Thread(const Thread&) = delete;
                Thread& operator=(const Thread&) = delete;
                Thread(Thread&&);
                Thread& operator=(Thread&&);
                ~Thread();
                
                bool operator==(const Thread& thread){return u_threadId==thread.u_threadId;};
                void start();
                void stop();
            
                bool isStart() const{return u_start;};
                void setThreadCallBack(const ThreadFunc& cb){u_threadFunc = cb;};
                pthread_t getThreadId() const{return u_threadId;};  
                const ThreadFunc& getThreadFunc() const{return u_threadFunc;};

            private:
                pthread_t u_threadId;
                bool u_start;
                ThreadFunc u_threadFunc;
        };
    }
}
                        
#endif

