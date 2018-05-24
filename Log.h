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
        typedef std::shared_ptr<Timer> TimerPtr;

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
            static void startWriteBack();
            static void stopWriteBack();

        private:
            void handleSwapEvent();
            void handleWriteBackEvent();
            void initWriteBack(); 

        private:
            bool u_start;
            int u_length;
            std::list<LogBuffer*> u_bufferList;
            base::MutexLock u_mutex;
            TimerPtr u_timer;
            
            static bool u_writeBackInit;
            static bool u_writeBackStart;
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
     *考虑多线程使用场景：
     * 多线程共享一个时间缓存肯定是要加锁的，所以决定让每个线程单独维护一个Time
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
            
            const char* format();//返回当时缓存的时间

        private:
            void handleSecTimerEvent();
            
        private:
            Timer u_secTimer; //每1s使用系统调用，更新一下本地的缓存
            base::MutexLock u_mutex;

            std::string u_data; //日期
            std::string u_time; //时间
            std::string u_utime; //微秒
    };
    
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
