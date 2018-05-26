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
    base::MutexLock LogBufferQueue::u_fileMutex;

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
    
    /*在析构的时候，将所有Buffer中的数据同意写入LogFile*/
    /*析构的时候不能用锁*/
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

        //将start关闭之后，Timer与WorkThread不再向LogBuffer中增添数据
        LogBuffer* current = u_bufferList.front();
        {
            base::MutexLockGuard guard(u_fileMutex);
            u_logFile.writen(current);
        }
    }
    
    /*全局的核心工作，将log写入缓冲区Buffer中,根据writeBuffer传回的参数进行
     * 详细的分析*/
    void LogBufferQueue::append(const char* message)
    {
        if(!u_start)
            return;
        LogBuffer* current = u_bufferList.front();
        int returned = writeInLogBuffer(current,message,strlen(message));
        
        switch(returned)
        {
            case -1:
            {
                perror("memcpy error!\n");
                unet::handleError(errno);
                break;
            }
            case 2:
            {
                perror("buffer not in use!\n");
                break;
            }
            case 1: /*这个Buffer已满，需要更换*/
            {
                handleSwapEvent();
                break;
            }
            case 3:
            case 0:
                break;
        }
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
            
            {
                base::MutexLockGuard guard(u_fileMutex);
                u_logFile.writen(buffer);;
            }
            alloc::deallocLogBuffer(buffer);
        }
    }
}
