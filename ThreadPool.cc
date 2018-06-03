/*************************************************************************
	> File Name: ThreadPool.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月27日 星期一 23时31分49秒
 ************************************************************************/

#include"ThreadPool.h"
#include"base/Timer.h"

namespace unet
{
    /*线程在线程池中有三种状态：
     * 0：内存已分配可以直接启用
     * 1：停止状态
     * n：已经启动*/
    ThreadPool::ThreadPool() :
        u_threadSize(0),
        u_startThreadSize(0),
        u_thread(),
        u_description()
    {   
        init();
    }

    ThreadPool::ThreadPool(ThreadPool&& pool) :
        u_threadSize(pool.u_threadSize),
        u_startThreadSize(pool.u_startThreadSize),
        u_thread(std::move(pool.u_thread)),
        u_description(std::move(pool.u_description))
    {};

    ThreadPool& ThreadPool::operator=(ThreadPool&& pool)
    {
        if(pool == *this)
            return *this;
        
        stopAllThread();

        u_threadSize = pool.u_threadSize;
        u_startThreadSize = pool.u_startThreadSize;
        u_thread = std::move(pool.u_thread);
        u_description = std::move(pool.u_description);

        return *this;
    }

    ThreadPool::~ThreadPool()
    {
        /*stopall中包括定时器的终止*/
        stopAllThread();
    }
    
    void ThreadPool::init()
    {
        /*预先初始化一定数目的线程*/
        /*怎么说呢，线程这种资源不像内存，重复利用的可能性为0*/
        /*init中不能用锁，因为在构造函数中调用*/
        /*预先给所有的线程分配内存资源，之后的启动工作无需在内存上花费时间*/
        for(int i=0;i<MAX_THREADS;i++)
        {
            u_thread[i] = std::make_shared<base::Thread>();
            u_description[i] = std::make_shared<ThreadDes>(std::make_tuple(0,"NULL",false)); 
        }

        u_threadSize = 0;
        u_startThreadSize = 0;
    }
    
    /*因为线程池维护的是和内存相关的动态的资源，所以每块内存都需要合适的标记*/
    /*从当前的空闲内存块中寻找一块可以直接存放线程本体的位置*/
    /*成功的话，返回可以存放线程本体的编号；没有空闲位置的话返回-1*/
    int ThreadPool::findAvailablePos()
    {
        if(u_threadSize == MAX_THREADS)
            return -1;
        for(int i=0;i<MAX_THREADS;i++) /*在已经有线程资源的数量上进行搜索*/
            if(std::get<0>(*u_description[i]) == 0 && 
                    std::get<2>(*u_description[i]) == false)    
                return i;      

        return -1;
    }
    
    /*
     * u_description和u_thread同时存在，同时消失
     *从线程池中删除一个线程的步骤：
     * 1.寻找tid对应的description和thread，进行相应的调整
     */
    int ThreadPool::addThread(const ThreadFunc& func,int size,const char* des)
    {
        /*线程池已满的话，直接返回*/
        if(u_threadSize == MAX_THREADS)
            return -1;

        int index = -1;
        for(int i=0;i<size;i++)
        {
            index = findAvailablePos();
            if(index == -1)
            {
                perror("There is no free space in Thread Pool!\n");
                return -1;
            }

            u_thread[index]->setThreadCallBack(func);
            
                /*添加线程但是未启动时：将pthread_t置为1*/
            std::get<0>(*u_description[index]) = 1;
            std::get<1>(*u_description[index]) = des;
            std::get<2>(*u_description[index]) = false;
            
            ++u_threadSize;
        }
        
        return 0;
    }
    
    int ThreadPool::addThread(std::shared_ptr<base::Thread>& thread,const char* des)
    {
        if(u_threadSize == MAX_THREADS)
            return -1;

        int index = findAvailablePos();
        if(index == -1)
            return -1;
        u_thread[index].reset();
        u_thread[index] = thread;
        if(thread->isStart())
            ++u_startThreadSize;
        ++u_threadSize;
        
        std::get<0>(*u_description[index]) = thread->isStart() ? thread->getThreadId() : 1;
        std::get<1>(*u_description[index]) = des;
        std::get<2>(*u_description[index]) = thread->isStart();
        
        return 0;
    }
    
    /*删除某个线程时，不对内存资源进行调整，只是将某个线程标记为立即可用*/
    int ThreadPool::deleteThread(pthread_t tid)
    {
        if(u_threadSize == 0)
            return -1;

        int index = 0;
        {
            for(;index < MAX_THREADS;index++)
            {
                if(std::get<0>(*u_description[index]) == tid)
                {
                    std::get<0>(*u_description[index]) = 0;
                    std::get<2>(*u_description[index]) = false;
                    break;
                }
            }
        }
        
        if(index == MAX_THREADS)
            return -1;

        if(u_thread[index])
        {
            if(u_thread[index]->isStart())
            {
                u_thread[index]->stop();
                --u_startThreadSize;
            }
            --u_threadSize;
        }
        
        return 0;
    }

    int ThreadPool::deleteThread(const std::string& str)
    {
        if(u_threadSize == 0)
            return -1;

        for(int index = 0;index < MAX_THREADS;index++)
        {
            if(std::get<1>(*u_description[index]) == str)
            {
                std::get<0>(*u_description[index]) = 0;
                std::get<2>(*u_description[index]) = false;
                
                if(u_thread[index]->isStart())
                {
                    u_thread[index]->stop();
                    --u_startThreadSize;
                }
                --u_threadSize;

            }
        }
        
        return 0;
    }

    int ThreadPool::deleteThreadNotStop(pthread_t tid)
    {
        if(u_threadSize == 0)
            return -1;

        int index = 0;
        {
            for(;index < MAX_THREADS;index++)
            {
                if(std::get<0>(*u_description[index]) == tid)
                {
                    std::get<0>(*u_description[index]) = 0;
                    std::get<2>(*u_description[index]) = false;
                    break;
                }
            }
        }
        
        if(index == MAX_THREADS)
            return -1;

        if(u_thread[index])
        {
            if(u_thread[index]->isStart())
                --u_startThreadSize;
            --u_threadSize;
        }
        
        return 0;

    }
    
    ThreadPool::ThreadPtr ThreadPool::getThread(int index)
    {
        if(index < 0 && index >= MAX_THREADS && !u_thread[index])
            return NULL;
        return u_thread[index];
    }

    ThreadPool::ThreadDesPtr ThreadPool::getThreadDes(int index)
    {
        if(index < 0 && index >= MAX_THREADS && !u_description[index])
            return NULL;
        return u_description[index]; 
    }
    
    /*线程池之间资源的动态迁移*/
    int ThreadPool::addThreadPool(ThreadPool& pool)
    {
        if(pool == *this)
            return 0;
        
        if(pool.u_threadSize == 0)
            return 0;

        int index = -1;
        for(int i=0;i<MAX_THREADS;i++)  /*遍历pool的空间*/
        {
            if(std::get<0>(*pool.u_description[i]) == 0)
                continue;

            index = findAvailablePos();/*从本地找到一块空间*/
            if(index == -1)
                return -1;
            u_thread[index] = pool.getThread(i);
            ++u_threadSize;
            if(u_thread[index]->isStart())
                ++u_startThreadSize;
            
            u_description[index] = pool.getThreadDes(i);

            pool.deleteThreadNotStop(u_thread[i]->getThreadId());
        }

        return 0; 
    }

    void ThreadPool::startAllThread()
    {
        if(u_threadSize == 0 || u_threadSize == u_startThreadSize)
            return;

        for(int i=0;i<MAX_THREADS;i++)
        {
            if(!std::get<2>(*u_description[i]) && std::get<0>(*u_description[i]) == 1)
            {
                u_thread[i]->start();
                std::get<0>(*u_description[i]) = u_thread[i]->getThreadId();
                std::get<2>(*u_description[i]) = true;
                
                ++u_startThreadSize;
            }
        }
    }

    void ThreadPool::stopAllThread()
    {
        if(u_startThreadSize == 0 || u_threadSize == 0)
            return;

        for(int i=0;i<MAX_THREADS;i++)
        {
            if(std::get<2>(*u_description[i]) && u_thread[i]->isStart())
            {
                u_thread[i]->stop();
                std::get<0>(*u_description[i]) = 1;
                std::get<2>(*u_description[i]) = false;
                --u_startThreadSize;
            }
        }
    }

    void ThreadPool::startThread(pthread_t tid)
    {
        if(u_threadSize == 0 || u_startThreadSize == u_threadSize)
            return;

        for(int i=0;i<MAX_THREADS;i++)
        {
            if(std::get<0>(*u_description[i]) == tid && 
                    !std::get<2>(*u_description[i]) && 
                    std::get<0>(*u_description[i]) == 1)
            {
                u_thread[i]->start();
                std::get<0>(*u_description[i]) = u_thread[i]->getThreadId();
                std::get<2>(*u_description[i]) = true;
                ++u_startThreadSize;
                break;
            }
        }
    }
    
    void ThreadPool::startThread(const std::string& tid)
    {
        if(u_threadSize == 0 || u_startThreadSize == u_threadSize)
            return;

        for(int i=0;i<MAX_THREADS;i++)
        {
            if(std::get<1>(*u_description[i]) == tid && 
                    !std::get<2>(*u_description[i]) && 
                    std::get<0>(*u_description[i]) == 1)
            {
                u_thread[i]->start();
                std::get<0>(*u_description[i]) = u_thread[i]->getThreadId();
                std::get<2>(*u_description[i]) = true;
                ++u_startThreadSize;
            }
        }
    }

    void ThreadPool::stopThread(pthread_t tid)
    {
        if(u_threadSize == 0 || u_startThreadSize == 0)
            return;

        for(int i=0;i<MAX_THREADS;i++)
        {
            if(std::get<0>(*u_description[i]) == tid && 
                    !std::get<2>(*u_description[i]))
            {
                u_thread[i]->stop();
                std::get<0>(*u_description[i]) = 1;
                std::get<2>(*u_description[i]) = false;
                --u_startThreadSize;
            }
        }
    }
    
    void ThreadPool::stopThread(const std::string& tid)
    {
        if(u_threadSize == 0 || u_startThreadSize == 0)
            return;

        for(int i=0;i<MAX_THREADS;i++)
        {
            if(std::get<1>(*u_description[i]) == tid && 
                    std::get<0>(*u_description[i]) > 1 &&
                    std::get<2>(*u_description[i]))
            {
                u_thread[i]->stop();
                std::get<0>(*u_description[i]) = 1;
                std::get<2>(*u_description[i]) = false;
                --u_startThreadSize;
            }
        }
    }
    
    void ThreadPool::setThreadDes(pthread_t tid,const char* des)
    {
        if(u_startThreadSize == 0 || u_threadSize == 0)
            return;

        for(int i=0;i<MAX_THREADS;i++)
        {
            if(std::get<0>(*u_description[i]) == tid)
            {
                std::get<1>(*u_description[i]) = des;
                return;
            }
        }
    }

    bool ThreadPool::threadInPool(pthread_t tid) const
    {
        if(u_startThreadSize == 0 || u_threadSize == 0)
            return false;

        for(int i=0;i<MAX_THREADS;i++)
            if(std::get<0>(*u_description[i]) == tid)
                return true;
        return false;
    }
    
    bool ThreadPool::threadInPool(const std::string& tid) const
    {
        if(u_startThreadSize == 0 || u_threadSize == 0)
            return false;
        
        for(int i=0;i<MAX_THREADS;i++)
            if(std::get<1>(*u_description[i]) == tid)
                return true;
        return false;
    }

    int ThreadPool::setThreadFunc(pthread_t tid,const ThreadFunc& func)
    {
        if(u_startThreadSize == 0 || u_threadSize == 0)
            return false;
        
        for(int i=0;i<MAX_THREADS;i++)
        {
            if(std::get<0>(*u_description[i]) == tid)
            {
                if(u_thread[i]->isStart())
                {
                    u_thread[i]->stop();
                    u_thread[i]->setThreadCallBack(func);
                    u_thread[i]->start();
                    
                    std::get<0>(*u_description[i]) = u_thread[i]->getThreadId();
                    return 0;
                }
            }
        }
        return -1;
    }
    
    int ThreadPool::setThreadFunc(const std::string& tid,const ThreadFunc& func)
    {
        if(u_startThreadSize == 0 || u_threadSize == 0)
            return false;
        
        for(int i=0;i<MAX_THREADS;i++)
        {
            if(std::get<1>(*u_description[i]) == tid)
            {
                if(u_thread[i]->isStart())
                {
                    u_thread[i]->stop();
                    u_thread[i]->setThreadCallBack(func);
                    u_thread[i]->start();
                    
                    std::get<0>(*u_description[i]) = u_thread[i]->getThreadId();
                }
            }
        }
        return 0;
    }
    
    void ThreadPool::printThreadPoolMessage()
    {
        printf("=========================================\n");
        printf("Thread Pool Message: \n");
        printf("Thread Size: %d\n",u_threadSize);
        printf("Start Thread Size: %d\n",u_startThreadSize);
        for(int i=0;i<MAX_THREADS;i++)
        {
            if(std::get<0>(*u_description[i]) == 0)
                continue;
            printf("Thread ID: %ld    Thread Description: %s    Is Started: %d\n",std::get<0>(*u_description[i]),std::get<1>(*u_description[i]).c_str(),std::get<2>(*u_description[i]));
        }
        printf("=========================================\n");
    }
    
    int ThreadPool::threadSize(const std::string& tid) const
    {
        if(u_startThreadSize == 0 || u_threadSize == 0)
            return false;
        int res = 0;
        for(int i=0;i<MAX_THREADS;i++)
            if(std::get<1>(*u_description[i]) == tid)
                ++res;
        return res;
    } 
}
