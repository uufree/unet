/*************************************************************************
	> File Name: LogBuffer.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月26日 星期六 06时06分43秒
 ************************************************************************/

#include"LogBuffer.h"

namespace unet
{
    std::list<base::LogBuffer*> LogBufferQueue::u_writeBackList;
    base::MutexLock LogBufferQueue::u_writeBackMutex;
    base::Condition LogBufferQueue::u_writeBackCond(u_writeBackMutex);
    LogFile LogBufferQueue::u_logFile;    
    base::Thread LogBufferQueue::u_writeThread(std::bind(&LogBufferQueue::WriteBackThread));
    
    LogBufferQueue::LogBufferQueue(int length) : 
        u_length(length),
        u_logNumbers(0),
        u_bufferList()
    {
        for(int i=0;i<u_length;++i)
            u_bufferList.push_back(alloc::allocLogBuffer());
    }

    LogBufferQueue::LogBufferQueue(LogBufferQueue&& log) :
        u_length(log.u_length),
        u_logNumbers(log.u_logNumbers),
        u_bufferList(std::move(log.u_bufferList))
    {}

    LogBufferQueue& LogBufferQueue::operator=(LogBufferQueue&& log)
    {   
        if(log == *this)
            return *this;
        
        clear();
        std::swap(u_bufferList,log.u_bufferList);
        u_length = log.u_length;
        u_logNumbers = log.u_logNumbers;
        
        return *this;
    }
    
    /*在析构的时候，将所有Buffer中的数据同意写入LogFile*/
    /*析构的时候不能用锁*/
    LogBufferQueue::~LogBufferQueue()
    {
        clear();
    }
    
    void LogBufferQueue::clear()
    {
        //将start关闭之后，Timer与WorkThread不再向LogBuffer中增添数据
        base::LogBuffer* current = u_bufferList.front();
        {
            base::MutexLockGuard guard(u_writeBackMutex);
            u_writeBackList.push_back(current);
            u_writeBackCond.notify();
        }
        u_bufferList.pop_front();   /*这个Buffer会在Log Thread中进行清理*/

        while(!u_bufferList.empty())
        {
            current = u_bufferList.front();
            alloc::deallocLogBuffer(current);
            u_bufferList.pop_front();
        }
    }
    
    /*全局的核心工作，将log写入缓冲区Buffer中,根据writeBuffer传回的参数进行
     * 详细的分析*/
    void LogBufferQueue::append(const char* message)
    {
        base::LogBuffer* current = u_bufferList.front();
        writeInLogBuffer(current,message,strlen(message));
        ++u_logNumbers;
        
        if(u_logNumbers == MAX_LOG)
        {
            {
                base::MutexLockGuard guard(u_writeBackMutex);
                u_writeBackList.push_back(current);
                u_writeBackCond.notify();
            }
            u_bufferList.pop_front();
            u_bufferList.push_back(alloc::allocLogBuffer());
            u_logNumbers = 0;
        }
    }
    
    /*Buffer->file的回写线程*/
    void LogBufferQueue::WriteBackThread()
    {
        std::list<base::LogBuffer*> logBufferList;
        base::LogBuffer* buffer = NULL;
        if(!u_logFile.isOpen())
            u_logFile.open();
        while(1)
        {
            base::MutexLockGuard guard(u_writeBackMutex);
            while(u_writeBackList.empty())
                u_writeBackCond.wait();
            logBufferList.splice(logBufferList.begin(),u_writeBackList,u_writeBackList.begin(),u_writeBackList.end());

            while(!logBufferList.empty())
            {
                buffer = logBufferList.front();
                u_logFile.writen(buffer);  
                logBufferList.pop_front();
                alloc::deallocLogBuffer(buffer);
            }
        }
    }
}
