/*************************************************************************
	> File Name: Log.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月20日 星期日 19时35分35秒
 ************************************************************************/

#include"Log.h"

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
        u_format(),
        u_buckets()
    {
        if(log.u_start)
        {
            log.stop();
            log.u_start = false;
        }
        u_buckets.swap(log.u_buckets);
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
    }
    
    void Log::start()
    {
        LogBufferQueue::startWriteThread();
        for(int i=0;i<HASH;i++)
            u_buckets.push_back(new LogBufferQueue());
        u_start = true;
    }

    void Log::stop()
    {
        LogBufferQueue::stopWriteThread();
        for(int i = 0;i < HASH;i++)
            delete u_buckets[i];
        u_start = false;
    }


    void Log::log(unsigned long tid,const char* message,const char* file,int line,LogLevel level)
    {
        if(!u_start)
            return;
        if(level < u_logLevel)
            return;
        
        LogBufferQueue* ptr = u_buckets[tid % 5];

        if(ptr != NULL)
        {
            ptr->append(u_format.formatLog(tid,level,message,file,line));
            std::cout << "log2" << std::endl;
        }
    }

    void Logg(unsigned long tid,const char* message,const char* file,int line,LogLevel level){LOG.log(tid,message,file,line,level);};
    void LogStart(){LOG.start();};
    void LogStop(){LOG.stop();};
    void SetLogLevel(LogLevel level){LOG.setLogLevel(level);}
}
