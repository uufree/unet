/*************************************************************************
	> File Name: LogBuffer.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月26日 星期六 06时06分43秒
 ************************************************************************/

#include"LogBuffer.h"
#include"base/Timer.h"
#include"Alloc.h"
#include"LogFile.h"

namespace unet
{
    bool LogBufferQueue::u_writeBackInit = false;
    bool LogBufferQueue::u_writeBackStart = false;
    std::list<LogBuffer*> LogBufferQueue::u_writeBackList;
    std::shared_ptr<Timer> LogBufferQueue::u_writeBackTimer(std::make_shared<Timer>(true,3)); 
    base::MutexLock LogBufferQueue::u_writeBackMutex;
    LogFile LogBufferQueue::u_logFile;

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
            base::MutexLockGuard guard(log.u_mutex);
            {
                base::MutexLockGuard guard(u_mutex);
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
           
            //将写入的任务交给LogFile
            u_logFile.writen(buffer);;

            alloc::deallocLogBuffer(buffer);
        }
    }
}
