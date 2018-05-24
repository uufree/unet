/*************************************************************************
	> File Name: Log.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月20日 星期日 19时35分35秒
 ************************************************************************/

#include"Log.h"
#include<sys/time.h>

namespace unet
{
    bool LogBufferQueue::u_writeBackInit = false;
    bool LogBufferQueue::u_writeBackStart = false;
    std::list<LogBuffer*> LogBufferQueue::u_writeBackList;
    std::shared_ptr<Timer> LogBufferQueue::u_writeBackTimer(std::make_shared<Timer>(true,3)); 
    base::MutexLock LogBufferQueue::u_writeBackMutex;
        
    LogBufferQueue::LogBufferQueue(int length) : 
        u_start(false),
        u_length(length),
        u_mutex(),
        u_timer(std::make_shared<Timer>(true,1))
    {
        for(int i=0;i<u_length;++i)
            u_bufferList.push_back(alloc::allocLogBuffer());
        u_timer->setTimeCallBack(std::bind(&LogBufferQueue::handleSwapEvent,this));
        if(!u_writeBackInit)
            initWriteBack();
    }

    LogBufferQueue::LogBufferQueue(LogBufferQueue&& log) :
        u_start(false),
        u_length(log.u_length),
        u_mutex(),
        u_timer(std::make_shared<Timer>(true,1))
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
        if(u_start)
            return;
        
        u_timer->start();
        u_start = true;
    }

    void LogBufferQueue::stop()
    {
        if(!u_start)
            return;

        u_timer->stop();
        u_start = false;
    }
    
    void LogBufferQueue::startWriteBack()
    {
        if(u_writeBackStart)
            return;
        u_writeBackTimer->start();
        u_writeBackStart = true;
    }

    void LogBufferQueue::stopWriteBack()
    {
        if(!u_writeBackStart)
            return;
        u_writeBackTimer->stop();
        u_writeBackStart = false;
    }
    
    void LogBufferQueue::LogBufferQueue::initWriteBack()
    {
        u_writeBackTimer->setTimeCallBack(std::bind(&LogBufferQueue::handleWriteBackEvent,this));
        u_writeBackInit = true;
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

    //LogFormat
    LogFormat::LogFormat() :
        u_secTimer(true,1),
        u_mutex(),
        u_data(),
        u_time(),
        u_utime()
    {
        u_secTimer.setTimeCallBack(std::bind(&LogFormat::handleSecTimerEvent,this));
        
        time_t now;
        if(::time(&now) < 0)
        {
            perror("::time error!\n");
            unet::handleError(errno);
        }
        struct tm* timenow = localtime(&now);
        u_data = std::to_string(timenow->tm_year+1900)+std::to_string(timenow->tm_mon+1)+std::to_string(timenow->tm_mday);
        u_time = std::to_string(timenow->tm_hour) + ":" + std::to_string(timenow->tm_min) + ":" + std::to_string(timenow->tm_sec) + ".";
        
        struct timeval val;
        ::gettimeofday(&val,NULL);
        u_utime = std::to_string(val.tv_usec) + "Z";
    };

    LogFormat::LogFormat(LogFormat&& format) :
        u_secTimer(true,1),
        u_mutex()
    {
        {
            base::MutexLockGuard guard(format.u_mutex);
            std::swap(u_data,format.u_data);
            std::swap(u_time,format.u_time);
            std::swap(u_utime,format.u_utime);
        }
    }
    
    LogFormat& LogFormat::operator=(LogFormat&& format)
    {
        {
            base::MutexLockGuard guard(format.u_mutex);
            {
                base::MutexLockGuard guard(u_mutex);
                std::swap(u_data,format.u_data);
                std::swap(u_time,format.u_time);
                std::swap(u_utime,format.u_utime);
            }
        }

        return *this;
    }
    
    LogFormat::~LogFormat()
    {
         
    }
}



