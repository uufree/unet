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
#include<assert.h>

namespace unet
{
    namespace thread
    {
        void* runInThread(void* arg)
        {
            typedef std::function<void ()> ThreadFunc;
            ThreadFunc* thread(static_cast<ThreadFunc*>(arg));

            (*thread)();
            return 0;
        };

        class Thread final
        {
            public:
                typedef std::function<void()> ThreadFunc;

                explicit Thread(const ThreadFunc& thread) : threadid(0),started(false),joined(false),threadfunc(thread)
                {};

                explicit Thread(ThreadFunc&& thread) : threadid(0),started(false),joined(false),threadfunc(std::move(thread))
                {};

                Thread(const Thread&) = delete;
                Thread& operator=(const Thread&) = delete;

                pthread_t getThreadid()
                {
                    return threadid;
                }   
                
                ~Thread()
                {
                    if(started && !joined)
                    {
                        pthread_detach(threadid);
                    }
                }

                int start()
                {
                    assert(!started);
                    started = true;
            
                    return pthread_create(&threadid,NULL,unet::thread::runInThread,&threadfunc);
                }

                int join()
                {
                    assert(started);
                    assert(!joined);
        
                    joined = true;

                    return pthread_join(threadid,NULL);
                }

            private:
                pthread_t threadid;
                bool started;
                bool joined;
                ThreadFunc threadfunc;
        };
    }

}
            
            
#endif

