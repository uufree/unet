/*************************************************************************
	> File Name: Thread.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年02月26日 星期日 00时35分15秒
 ************************************************************************/

#include<pthread.h>
#include<functional>
#include<assert.h>
#include<iostream>
#include<unistd.h>
#include<sys/syscall.h>

namespace unet
{
    namespace current
    {
        __thread pthread_t threadid_;
        __thread pid_t pid_;

        inline pid_t pid()
        {
            pid_ = ::syscall(SYS_gettid);
            return pid_;
        }
    }

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

            pthread_t getThreadid() const
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
            
                return pthread_create(&threadid,NULL,unet::runInThread,&threadfunc);
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

class node
{
    public:
        node() : m(10),lock(),cond(lock){};
        ~node() {};

        void wait()
        {
            unet::MutexLockGuard guard(lock);
            while(m > 0)
                cond.wait();
        }

        void countDown()
        {
            unet::MutexLockGuard guard(lock);
            --m;
            if(m == 0)
                cond.notify();
        }
        
        int getCount()
        {
            unet::MutexLockGuard guard(lock);
            return m;
        };

    private:
        int m;
        unet::MutexLock lock;
        unet::Condition cond;
};

class node mkl;

void func(int m,int n,int x)
{
    while(1)
    {
        if(mkl.getCount() != 0)
        {
            mkl.countDown();
            std::cout << "thread runing!" << std::endl;
            ::sleep(1);
        }
    }
}


int main(int argc,char** argv)
{
    unet::Thread thread(std::bind(&func,3,1,4));
    thread.start();
    
    while(1)
    {
        mkl.wait();
        std::cout << "main wake up!" << std::endl;
        sleep(1);
    }
    return 0;
}







































