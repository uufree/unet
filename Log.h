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

#include"Alloc.h"
#include"base/Mutex.h"
#include"base/Thread.h"
#include"base/global.h"
#include"base/Timer.h"

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
    
    /*设计目的：
     * 细化LogBuffer锁的粒度，所有的Queue又共享一个WriteBackQueue。
     * 每个工作线程会向线程专属所属的LogBuffer写入日志，定时清理LogBuffer。
     * 定时清理WriteBackQueue。
     */ 
    class LogBufferQueue final
    {
        typedef std::shared_ptr<base::Timer> TimerPtr;
        typedef std::function<void(TimerPtr)> AddTimerCallBack;
        typedef std::function<void(TimerPtr)> DeleteTimerCallBack;

        public:
            LogBufferQueue(int length = 2);
            LogBufferQueue(const LogBufferQueue&) = delete;
            LogBufferQueue& operator=(const LogBufferQueue&) = delete;
            LogBufferQueue(LogBufferQueue&&);
            LogBufferQueue& operator=(LogBufferQueue&&);
            ~LogBufferQueue();
            
            bool operator==(const LogBufferQueue& log){return u_bufferList.front() == log.u_bufferList.front();};

            void start();
            void stop();
            void setADCallBack(const AddTimerCallBack&,const DeleteTimerCallBack&);

        private:
            void handleSwapEvent();
            void handleWriteBackEvent();

        private:
            bool u_start;
            int u_length;
            std::list<LogBuffer*> u_bufferList;
            base::MutexLock u_mutex;
            TimerPtr u_timer;
            
            static bool u_adInit;
            static AddTimerCallBack u_addInTimerCallBack;
            static DeleteTimerCallBack u_deleteTimerCallBack;

            static bool u_writeBackInit;
            static std::list<LogBuffer*> u_writeBackList;
            static TimerPtr u_writeBackTimer;
            static base::MutexLock u_writeBackMutex;
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
