/*************************************************************************
	> File Name: Log.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月20日 星期日 19时35分35秒
 ************************************************************************/

#include"Log.h"

namespace unet
{
    bool LogBufferQueue::u_adInit = false;

    bool LogBufferQueue::u_writeBackInit = false;
    std::list<LogBuffer*> LogBufferQueue::u_writeBackList;
    std::shared_ptr<base::Timer> LogBufferQueue::u_writeBackTimer(std::make_shared<base::Timer>(true,3)); 
    base::MutexLock LogBufferQueue::u_writeBackMutex;
        
    LogBufferQueue::LogBufferQueue(int length) : 
        u_start(false),
        u_length(length),
        u_mutex(),
        u_timer(std::make_shared<base::Timer>(true,1))
    {
        for(int i=0;i<u_length;++i)
            u_bufferList.push_back(alloc::allocLogBuffer());
        u_timer->setTimeCallBack(std::bind(&LogBufferQueue::handleSwapEvent,this));
        if(!u_writeBackInit)
        {
            u_writeBackTimer->setTimeCallBack(std::bind(&LogBufferQueue::handleWriteBackEvent,this));
            u_writeBackInit = true;
        }
    }

    LogBufferQueue::LogBufferQueue(LogBufferQueue&& log) :
        u_start(false),
        u_length(log.u_length),
        u_mutex(),
        u_timer(std::make_shared<base::Timer>(true,1))
    {
        if(log.u_start)
            log.stop();

        {
            base::MutexLockGuard guard(log.u_mutex);
            std::swap(log.u_bufferList,u_bufferList);
        }
        
        u_timer->setTimeCallBack(std::bind(&LogBufferQueue::handleSwapEvent,this));
        if(!u_writeBackInit)
        {
            u_writeBackTimer->setTimeCallBack(std::bind(&LogBufferQueue::handleWriteBackEvent,this));
            u_writeBackInit = true;
        }
    }
    
    LogBufferQueue& LogBufferQueue::operator=(LogBufferQueue&& log)
    {   
        if(log == *this)
            return *this;
        
        if(u_start)
            stop();
        if(log.u_start)
            log.stop();

        {
            base::MutexLockGuard guard(u_mutex);
            {
                base::MutexLockGuard guard(log.u_mutex);
                std::swap(u_bufferList,log.u_bufferList);
                std::swap(u_length,log.u_length);
            }
        }
        
        return *this;
    }
    
    LogBufferQueue::~LogBufferQueue()
    {
        if(u_start)
            stop();
    }
    
    void LogBufferQueue::start()
    {
        if(u_adInit)
            u_addInTimerCallBack(u_timer);
        u_start = true;
    }

    void LogBufferQueue::stop()
    {
        if(u_adInit)
            u_deleteTimerCallBack(u_timer);
        u_start = false;
    }

    void LogBufferQueue::setADCallBack(const AddTimerCallBack& add,const DeleteTimerCallBack& del)
    {
        u_addInTimerCallBack = add;
        u_deleteTimerCallBack = del;
        u_adInit = true;
    }


    void LogBufferQueue::handleSwapEvent()
    {
        LogBuffer* buffer = alloc::allocLogBuffer();

        {
            base::MutexLockGuard guard(u_mutex);
            u_bufferList.push_back(buffer); 
            
            {
                base::MutexLockGuard guard(u_writeBackMutex);
                u_writeBackList.push_back(u_bufferList.front());
                u_bufferList.pop_front();
            }
        }
    }

    void LogBufferQueue::handleWriteBackEvent()
    {
        std::list<LogBuffer*> handleList;
        
        {
            base::MutexLockGuard guard(u_writeBackMutex);
            handleList.splice(handleList.begin(),u_writeBackList,u_writeBackList.begin(),u_writeBackList.end());
        }
        
        LogBuffer* buffer = NULL;
        if(!handleList.empty())
        {
            buffer = handleList.front();
            handleList.pop_front();
            /*将Buffer中的数据写入LogFile中
            
              !!!!!!!!!!!!!!!
              !!!!!!!!!!!!!!!
              !!!!!!!!!!!!!!!
            
            以上是Buffer写入过程*/
            alloc::deallocLogBuffer(buffer);
        }
    }
    
    void Log::LogFunc(pid_t tid,const char* str)
    {
        LogBufferQueuePtr ptr = u_buckets[tid];

    }

}

