/*************************************************************************
	> File Name: Log.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月20日 星期日 19时35分35秒
 ************************************************************************/

#include"Log.h"
#include"LogBuffer.h"

namespace unet
{
    Log::Log() :
        u_start(false),
        u_logLevel(INFO),
        u_format(),
        u_buckets()
    {};
    
    Log::Log(Log&& log) : 
        u_start(false),
        u_logLevel(log.u_logLevel),
        u_format()
    {
        if(log.u_start)
        {
            log.stop();
            log.u_start = false;
        }
        u_buckets = std::move(log.u_buckets);
    }

    Log& Log::operator=(Log&& log)
    {
        if(log == *this)
            return *this;
        
        if(u_start)
            u_start = false;
        if(log.u_start)
            log.u_start = false;

        u_logLevel = log.u_logLevel;
        std::swap(u_buckets,log.u_buckets);

        return *this;
    }

    Log::~Log()
    {
        if(u_start)
            stop();
        u_buckets.clear();
    }
    
    void log(pid_t tid,const char* message,const char* file,int line,LogLevel level,Log& log)
    {
        if(!log.u_start)
            return;
        if(level > log.u_logLevel)
            return;

        Log::LogBufferQueuePtr ptr = log.u_buckets[tid];
        if(ptr)
            ptr->append(log.u_format.formatLog(tid,level,message,file,line));
    }

    void Log::addWorkThread(int tid)
    {
        if(u_start)
            return;
        
        LogBufferQueuePtr ptr = u_buckets[tid];
        if(ptr)
            return;
        u_buckets.insert(std::make_pair(tid,std::make_shared<LogBufferQueue>())); 
    }

    void Log::deleteWorkThread(int tid)
    {
        if(u_start)
            return;
        LogBufferQueuePtr ptr = u_buckets[tid];
        if(ptr)
            u_buckets.erase(tid);
    }

    void Log::start()
    {
        if(u_start)
            return;

        for(auto iter=u_buckets.begin();iter!=u_buckets.end();++iter)
        {
            iter->second->start();
            iter->second->startWriteBack();
        }
        u_start = true;
    }

    void Log::stop()
    {
        if(!u_start)
            return;

        for(auto iter=u_buckets.begin();iter!=u_buckets.end();++iter)
        {
            iter->second->stop();
            iter->second->stopWriteBack();
        }
        u_start = false;
    }
}
