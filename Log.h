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

#include"Alloc.h"
#include"base/Mutex.h"
#include"base/Thread.h"
#include"base/global.h"

namespace unet
{
    enum LogLevel
    {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
        NUM_LOG_LEVELS
    };
    
    struct LogBufferQueue
    {
        LogBuffer* u_head;
        LogBuffer* u_current;
        int u_length;
        base::MutexLock u_mutex;
        
        LogBufferQueue(int length) : 
            u_head(NULL),
            u_current(NULL),
            u_length(length),
            u_mutex()
        {
            LogBuffer* head = alloc::allocLogBuffer();
            u_head = head;
            u_current = head;
            head = head->u_next;
            for(int i=0;i<length-1;++i)
            {
                head = alloc::allocLogBuffer();
                head = head->u_next;
            }
        }
    };
    
    namespace log
    {
        void LogFunc(pid_t,short);
        void LogFunc(pid_t,unsigned short);
        void LogFunc(pid_t,int);
        void LogFunc(pid_t,unsigned int);
        void LogFunc(pid_t,double);
        void LogFunc(pid_t,float);
        void LogFunc(pid_t,long);
        void LogFunc(pid_t,unsigned long);
        void LogFunc(pid_t,unsigned long long);
        void LogFunc(pid_t,long long);
        void LogFunc(pid_t,const char*);
        void LogFunc(pid_t,const std::string&);
    }

    class Log final
    {
        typedef std::unordered_map<int,LogBufferQueue> HashBuckets;
        typedef void (*LogFuncS)(pid_t,short);
        typedef void (*LogFuncUS)(pid_t,unsigned short);
        typedef void (*LogFuncI)(pid_t,int);
        typedef void (*LogFuncUI)(pid_t,unsigned int);
        typedef void (*LogFuncD)(pid_t,double);
        typedef void (*LogFuncF)(pid_t,float);
        typedef void (*LogFuncUL)(pid_t,unsigned long);
        typedef void (*LogFuncULL)(pid_t,unsigned long long);
        typedef void (*LogFuncLL)(pid_t,long long);
        typedef void (*LogFuncCC)(pid_t,const char*);
        typedef void (*LogFuncCS)(pid_t,const std::string&);
        typedef void (*LogFuncL)(pid_t,long);
        
        friend void LogFunc(pid_t,short);
        friend void LogFunc(pid_t,unsigned short);
        friend void LogFunc(pid_t,int);
        friend void LogFunc(pid_t,unsigned int);
        friend void LogFunc(pid_t,double);
        friend void LogFunc(pid_t,float);
        friend void LogFunc(pid_t,long);
        friend void LogFunc(pid_t,unsigned long);
        friend void LogFunc(pid_t,unsigned long long);
        friend void LogFunc(pid_t,long long);
        friend void LogFunc(pid_t,const char*);
        friend void LogFunc(pid_t,const std::string&);
        
        public:
            Log(LogLevel level);
            Log(const LogLevel&) = delete;
            Log& operator=(const Log&) = delete;
            Log(Log&&);
            Log& operator=(Log&&);
            ~Log();
            
            Log& operator<<(LogFuncS);
            Log& operator<<(LogFuncUS);
            Log& operator<<(LogFuncI);
            Log& operator<<(LogFuncUI);
            Log& operator<<(LogFuncD);
            Log& operator<<(LogFuncF);
            Log& operator<<(LogFuncUL);
            Log& operator<<(LogFuncULL);
            Log& operator<<(LogFuncLL);
            Log& operator<<(LogFuncCC);
            Log& operator<<(LogFuncCS);
            Log& operator<<(LogFuncL);
            
        private:
            void append(const char*);

        private:    
            LogLevel u_logLevel;
            static HashBuckets u_buckets;
    };
}

#endif
