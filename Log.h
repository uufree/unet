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
     *  LogFile：负责LogFile的滚动,以及将LogBuffer写入到具体的Log文件中
     *  LogFormat：格式化字符串，缓存一些时间数据，避免重复的系统调用
     *  LogBufferQueue：LogBuffer实体，缓存写入的Log
     *  Log：外部接口，可以调整Log等级
     */
    class Log final
    {
        typedef std::shared_ptr<LogBufferQueue> LogBufferQueuePtr;
        typedef std::unordered_map<int,LogBufferQueuePtr> HashBuckets;
        friend void log(pid_t,const char*,const char*,int,LogLevel,Log&);
        
        public:
            explicit Log();
            Log(const Log&) = delete;
            Log& operator=(const Log&) = delete;
            Log& operator=(Log&&);
            Log(Log&&);
            ~Log();
            
            bool operator==(const Log& log){return u_buckets.begin()->first == log.u_buckets.begin()->first && u_buckets.size() == log.u_buckets.size();};

            void setLogLevel(LogLevel level){u_logLevel = level;};
            LogLevel level() const{return u_logLevel;};
            void addWorkThread(int);
            void deleteWorkThread(int);
            void start();
            void stop();
            bool isStart() const{return u_start;};

        private:
            bool u_start;
            LogLevel u_logLevel;
            LogFormat u_format;
            HashBuckets u_buckets;
    };
    
    /*注意，将来在这块填充TcpServer的全局静态Log*/
    void log(pid_t,const char*,const char*,int,LogLevel,Log&);
    

#define LOG_TRACE(tid,str) if(Logger::u_logLevel <= TRACE) \
    log(tid,str,__FILE__,__LINE__,TRACE)

#define LOG_DEBUG(tid,str) if(Logger::u_logLevel <= DEBUG) \
    log(tid,str,__FILE__,__LINE__,DEBUG)

#define LOG_INFO(tid,str) if(Logger::u_logLevel <= INFO) \
    log(tid,str,__FILE__,__LINE__,INFO)

#define LOG_WARN(tid,str) if(Logger::u_logLevel <= WARN) \
    log(tid,str,__FILE__,__LINE__,WARN)

#define LOG_ERROR(tid,str) if(Logger::u_logLevel <= ERROR) \
    log(tid,str,__FILE__,__LINE__,ERROR)

#define LOG_FATAL(tid,str) if(Logger::u_logLevel <= FATAL) \
    log(tid,str,__FILE__,__LINE__,FATAL)
}



#endif
