/*************************************************************************
	> File Name: Thread.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年02月26日 星期日 00时35分15秒
 ************************************************************************/

#ifndef _THREAD_H
#define _THREAD_H

#include<functional>

/*2018.05.29 测试完成*/

namespace unet
{
    namespace base
    {
        /*封装Pthread函数，对象的生命期决定线程的生命期*/
        /*每个线程管理自己的资源，启动即分离*/
        /*由于启动时已经进行分离，使用join函数已经失效，所以使用cancle取消*/
        
        /*是否需要向线程注册线程资源清理函数?
         * 使用cancle的取消线程运行，但是线程仍然存在于分离状态，线程函数中存在的
         * 对象资源依旧存在一个明显的生命周期，所以暂时不需要注册资源清理
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
                /*pthread_t = unsigned long*/
                pthread_t u_threadId;
                bool u_start;
                ThreadFunc u_threadFunc;
        };
    }
}
                        
#endif

