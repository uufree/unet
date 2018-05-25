/*************************************************************************
	> File Name: Log.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月20日 星期日 19时35分30秒
 ************************************************************************/

#ifndef _LOG_H
#define _LOG_H

#include<memory>
#include<unordered_map>
#include<queue>
#include<string>
#include<list>

#include"LogFormat.h"

namespace unet
{
    class LogBufferQueue;

    /*
     * 整个Log由以下几部分组成：
     *  LogFile：负责LogFile的滚动
     *  LogFormat：
     *  LogBufferQueue：
     */
    class Log final
    {
        typedef std::shared_ptr<LogBufferQueue> LogBufferQueuePtr;
        typedef std::unordered_map<int,LogBufferQueuePtr> HashBuckets;
        
        public:
            explicit Log();
            Log(const Log&) = delete;
            Log& operator=(const Log&) = delete;
            Log& operator=(Log&&);
            Log(Log&&);
            ~Log();

            static void log(pid_t,const char*,int,const char*,LogLevel);

        private:
            LogLevel u_logLevel;
            LogFormat u_format;
            static HashBuckets u_buckets;
    };

#define LOG_TRACE(tid,str) if(Logger::u_logLevel <= TRACE) \
    Log::log(tid,str,__FILE__,__LINE__,TRACE)

#define LOG_DEBUG(tid,str) if(Logger::u_logLevel <= DEBUG) \
    Log::log(tid,str,__FILE__,__LINE__,DEBUG)

#define LOG_INFO(tid,str) if(Logger::u_logLevel <= INFO) \
    Log::log(tid,str,,__FILE__,__LINE__,INFO)

#define LOG_WARN(tid,str) if(Logger::u_logLevel <= WARN) \
    Log::log(tid,str,__FILE__,__LINE__,WARN)

#define LOG_ERROR(tid,str) if(Logger::u_logLevel <= ERROR) \
    Log::log(tid,str,__FILE__,__LINE__,ERROR)

#define LOG_FATAL(tid,str) if(Logger::u_logLevel <= FATAL) \
    Log::log(tid,str,__FILE__,__LINE__,FATAL)
}



#endif
