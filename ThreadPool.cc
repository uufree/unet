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
    ThreadPool::ThreadPool() :
        u_threadSize(0),
        u_startSize(0),
        u_thread(),
        u_mutex(),
        u_description(),
        u_timer(std::make_shared<Timer>(true,30)),
        u_stopList()
    {   
        init();
    }

    ThreadPool::ThreadPool(ThreadPool&& pool)
    {
        /*在两个线程池之间进行资源迁移时，停止定时器*/
        /*定时器不能被迁移，因为有注册this指针*/
        /*定时器的callback在startall时注册，避免提早暴露this指针*/
        pool.u_timer->stop();

        {
            base::MutexLockGuard guard(pool.u_mutex);
            u_description = std::move(pool.u_description);
            u_stopList = std::move(pool.u_stopList);
        }
        
        u_timer = std::make_shared<Timer>(true,30);
        u_threadSize = pool.u_threadSize;
        u_startSize = pool.u_startSize;
        u_thread = std::move(pool.u_thread);
        u_mutex = base::MutexLock();
    };

    ThreadPool& ThreadPool::operator=(ThreadPool&& pool)
    {
        if(pool == *this)
            return *this;
        
        pool.u_timer->stop();
        {
            /*在unet中不成文的规矩：先拿到外部对象锁，再拿本地对象锁*/
            base::MutexLockGuard guard(pool.u_mutex);
            {
                base::MutexLockGuard guard(u_mutex);
                u_description = std::move(pool.u_description);
                u_stopList = std::move(pool.u_stopList);
            }
        }
        
        u_threadSize = pool.u_threadSize;
        u_startSize = pool.u_startSize;
        u_thread = std::move(pool.u_thread);

        return *this;
    }

    ThreadPool::~ThreadPool()
    {
        /*stopall中包括定时器的终止*/
        stopAll();
    }
    
    void ThreadPool::init()
    {
        /*预先初始化一定数目的线程*/
        /*怎么说呢，线程这种资源不像内存，重复利用的可能性为0*/
        /*init中不能用锁，因为在构造函数中调用*/
        for(int i=0;i<MAX_THREADS;i++)
        {
            if(i < INIT_THREADS)
            {
                u_thread[i] = std::make_shared<base::Thread>();
                u_description[i] = std::make_shared<ThreadDes>(std::make_tuple(-1,"NULL",false)); 
            }
            else
            {
                u_thread[i] = NULL;
                u_description[i] = NULL;
            }
        }

        u_threadSize = INIT_THREADS;
        u_startSize = 0;
    }
    
    /*
     * 完成两项工作：
     * 1.首先检测与stopList中的编号是否依旧匹配，匹配删除，不匹配不变
     * 2.重新填充stopList
     */
    void ThreadPool::handleStopList()
    {
        base::MutexLockGuard guard(u_mutex);
        /*30s之前线程状态为false，30s之后如果依旧为false的话，可以复用*/
        for(auto iter=u_stopList.begin();iter!=u_stopList.end();++iter)
        {
            if(std::get<2>(*u_description[*iter]) == false) 
                std::get<0>(*u_description[*iter]) = -1; 
        }
        
        u_stopList.clear();
        for(int i=0;i<u_threadSize;i++)
        {
            if(std::get<0>(*u_description[i]) && 
                    std::get<2>(*u_description[i]) == false)
                u_stopList.push_back(i);
        }
    }
    
    int ThreadPool::findAvailablePos()
    {
        base::MutexLockGuard guard(u_mutex);
        for(int i=0;i<u_threadSize;i++)
            if(u_description[i] && 
                    std::get<0>(*u_description[i]) == (unsigned long)-1 && 
                    std::get<2>(*u_description[i]) == false)
                return i;      

        return -1;
    }
    
    int ThreadPool::findFreePos()
    {
        for(int i=0;i<u_threadSize;i++)
            if(!u_thread[i])
                return i;
        return u_threadSize;
    }

    /*
     * 在添加线程之前首先需要说明一点：若一个线程的tid=-1，则可以直接使用
     * 存在这样一种情况，一个线程启动之后，被停止了，之后不再运行。于是，tid
     * 一直被占用，怎么办呢？
     * 启动一个定时器，每30s检测一次已经停止，但是资源被占用的tid，重新初始化，
     * 使之可以被复用
     */

    /*
     *向线程池中添加一个线程的步骤：
     * 1.首先搜索pthread_t为-1的线程,找到之后直接使用即可
     * 2.如果不存在，重添加
     */

    /*
     * u_description和u_thread同时存在，同时消失
     *从线程池中删除一个线程的步骤：
     * 1.寻找tid对应的description和thread，进行相应的调整
     */
    const ThreadPool::TidList& ThreadPool::addThread(const ThreadFunc& func,int size,const char* des)
    {
        /*线程池已满的话，直接返回*/
        if(u_threadSize == MAX_THREADS)
            return u_description;

        int index = -1;
        for(int i=0;i<size;i++)
        {
            index = findAvailablePos();
            if(index == -1)
            {
                index = findFreePos();
                u_thread[index] = std::make_shared<base::Thread>();
                ++u_threadSize;
            }

            u_thread[index]->setThreadCallBack(func);
            
            {
                base::MutexLockGuard guard(u_mutex);
                if(!u_description[index])
                    u_description[index] = std::make_shared<ThreadDes>(std::make_tuple(-1,des,false));
                else
                {
                    std::get<0>(*u_description[index]) = -1;
                    std::get<1>(*u_description[index]) = des;
                    std::get<2>(*u_description[index]) = false;
                }
            }
        }
        
        return u_description;
    }
    
    const ThreadPool::TidList& ThreadPool::addThread(std::shared_ptr<base::Thread>& thread,const char* des)
    {
        if(u_threadSize == MAX_THREADS)
            return u_description;

        int index = findFreePos();
        u_thread[index] = thread;
        if(thread->isStart())
            ++u_startSize;

        {
            base::MutexLockGuard guard(u_mutex);
            u_description[index] = std::make_shared<ThreadDes>(std::make_tuple(thread->getThreadId(),des,thread->isStart()));
        }
        ++u_threadSize;
        return u_description;
    }

    const ThreadPool::TidList& ThreadPool::deleteThread(pthread_t tid)
    {
        int index = 0;
        {
            base::MutexLockGuard guard(u_mutex);
            for(;index < u_threadSize;index++)
            {
                if(std::get<0>(*u_description[index]) == tid)
                {
                    u_description[index].reset();
                    break;
                }
            }
        }
        
        if(index == u_threadSize)
            return u_description;

        if(u_thread[index])
        {
            if(u_thread[index]->isStart())
            {
                u_thread[index]->stop();
                --u_startSize;
            }
            
            u_thread[index].reset();
            --u_threadSize;
        }
        
        return u_description;
    }
    
    ThreadPool::ThreadPtr ThreadPool::getThread(int index)
    {
        if(index < 0 && index >= MAX_THREADS && index >= u_threadSize && !u_thread[index])
            return NULL;
        return u_thread[index];
    }

    ThreadPool::ThreadDesPtr ThreadPool::getThreadDes(int index)
    {
        if(index < 0 && index >= MAX_THREADS && index >= u_threadSize && !u_description[index])
            return NULL;
        return u_description[index]; 
    }

    const ThreadPool::TidList& ThreadPool::addThreadPool(ThreadPool& pool)
    {
        if(pool == *this)
            return u_description;
        
        int index = -1;
        for(int i=0;i<pool.threadSize();i++)
        {
            if(u_threadSize >= MAX_THREADS)
                break;
            index = findFreePos();
            u_thread[index] = pool.getThread(i);
            if(u_thread[index]->isStart())
                ++u_startSize;
            ++u_threadSize;
            {
                base::MutexLockGuard guard(u_mutex);
                u_description[index] = pool.getThreadDes(index);
            }
            pool.deleteThread(u_thread[index]->getThreadId());
        }
        return u_description; 
    }

    void ThreadPool::startAll()
    {
        for(int i=0;i<u_threadSize;i++)
        {
            if(!std::get<2>(*u_description[i]) && !u_thread[i]->isStart())
            {
                u_thread[i]->start();
                {
                    base::MutexLockGuard guard(u_mutex);
                    std::get<0>(*u_description[i]) = u_thread[i]->getThreadId();
                    std::get<2>(*u_description[i]) = true;
                }
                ++u_startSize;
            }
        }

        if(u_timer->hasCallBack())
            u_timer->setTimeCallBack(std::bind(&ThreadPool::handleStopList,this));
        u_timer->start();
    }

    void ThreadPool::stopAll()
    {
        for(int i=0;i<u_threadSize;i++)
        {
            if(std::get<2>(*u_description[i]) && u_thread[i]->isStart())
            {
                u_thread[i]->stop();
                {
                    base::MutexLockGuard guard(u_mutex);
                    std::get<2>(*u_description[i]) = false;
                }
                --u_startSize;
            }
        }

        u_timer->stop();
    }

    void ThreadPool::start(pthread_t tid)
    {
        for(int i=0;i<u_threadSize;i++)
        {
            if(std::get<0>(*u_description[i]) == tid && 
                    !std::get<2>(*u_description[i]) && 
                    !u_thread[i]->isStart())
            {
                u_thread[i]->start();
                {
                    base::MutexLockGuard guard(u_mutex);
                    std::get<0>(*u_description[i]) = u_thread[i]->getThreadId();
                    std::get<2>(*u_description[i]) = true;
                }
                ++u_startSize;
            }
        }
    }
    
    void ThreadPool::stop(pthread_t tid)
    {
        for(int i=0;i<u_threadSize;i++)
        {
            if(std::get<0>(*u_description[i]) == tid && 
                    !std::get<2>(*u_description[i]) && 
                    !u_thread[i]->isStart())
            {
                u_thread[i]->stop();
                {
                    base::MutexLockGuard guard(u_mutex);
                    std::get<2>(*u_description[i]) = false;
                }
                --u_startSize;
            }
        }
    }
    
    void ThreadPool::setThreadDes(pthread_t tid,const char* des)
    {
        for(int i=0;i<u_threadSize;i++)
        {
            if(std::get<0>(*u_description[i]) == tid)
            {
                base::MutexLockGuard guard(u_mutex);
                std::get<1>(*u_description[i]) = des;
                return;
            }
        }
    }

    bool ThreadPool::inPool(pthread_t tid) const
    {
        for(int i=0;i<u_threadSize;i++)
            if(std::get<0>(*u_description[i]) == tid)
                return true;
        return false;
    }
    
    void ThreadPool::setThreadFunc(pthread_t tid,const ThreadFunc& func)
    {
        for(int i=0;i<u_threadSize;i++)
        {
            if(std::get<0>(*u_description[i]) == tid)
            {
                if(u_thread[i]->isStart())
                {
                    u_thread[i]->stop();
                    {
                        base::MutexLockGuard guard(u_mutex);
                        std::get<2>(*u_description[i]) = false;
                    }
                    --u_startSize;
                }
                u_thread[i]->setThreadCallBack(func);
                return;
            }
        }
    }
}
