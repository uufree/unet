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
    
    /*
     * 具体的日志格式参照muduo提供的日志格式：
     * 日期-时间-微秒-线程-级别-正文-源文件名-行号（以空格隔开）
     * 20180522 21:20:35.125880Z 2501 INFO hello,world Poll.cc:78 
     */
    
    /*设计目的：
     * 使用这个类缓存上述的信息，每次从Log的<<获取一个字符串的时候，将缓存的数据重
     * 组成字符串，返回给Log进行处理。主要还是避免重复的系统调用，缓存时间等的信息
     */
    class LogFormat final
    {
        public:
            explicit LogFormat();  
            LogFormat(const LogFormat&) = delete;
            LogFormat& operator=(const LogFormat&) = delete;
            LogFormat(LogFormat&&);
            LogFormat& operator=(LogFormat&&);
            ~LogFormat();
        
            

        private:
            base::Timer u_minTimer; //每60s使用系统调用，更新各种数据
            base::Timer u_secTimer; //每1s处理一次，不使用系统调用
            
            std::string u_data; //日期
            std::string u_time; //时间
            std::string u_utime; //微秒
    };
    
    // Log << Log::LogFunc(pid_t,const char*);
    class Log final
    {
        typedef std::shared_ptr<LogBufferQueue> LogBufferQueuePtr;
        typedef std::unordered_map<int,LogBufferQueuePtr> HashBuckets;
        typedef void (*LogFuncCC)(pid_t,const char*);
        
        public:
            Log(LogLevel level);
            Log(const LogLevel&) = delete;
            Log& operator=(const Log&) = delete;
            Log(Log&&);
            Log& operator=(Log&&);
            ~Log();
            
            Log& operator<<(LogFuncCC);
            
            static void LogFunc(pid_t,const char*);

        private:
            void append(const char*);

        private:    
            static HashBuckets u_buckets;
    };

    struct Logger
    {
        static Log& log(const char*,int,LogLevel)
        {
            
            return u_log;
        }

        LogLevel u_logLevel;
        static Log u_log;
    };

#define LOG_TRACE if(Logger::u_logLevel <= TRACE) \
    Logger::log(__FILE__,__LINE__,TRACE)

#define LOG_DEBUG if(Logger::u_logLevel <= DEBUG) \
    Logger::log(__FILE__,__LINE__,DEBUG)

#define LOG_INFO if(Logger::u_logLevel <= INFO) \
    Logger::log(__FILE__,__LINE__,INFO)

#define LOG_WARN if(Logger::u_logLevel <= WARN) \
    Logger::log(__FILE__,__LINE__,WARN)

#define LOG_ERROR if(Logger::u_logLevel <= ERROR) \
    Logger::log(__FILE__,__LINE__,ERROR)

#define LOG_FATAL if(Logger::u_logLevel <= FATAL) \
    Logger::log(__FILE__,__LINE__,FATAL)
}



#endif
