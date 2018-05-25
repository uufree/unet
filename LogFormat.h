/*************************************************************************
	> File Name: LogFormat.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月26日 星期六 06时01分22秒
 ************************************************************************/

#ifndef _LOGFORMAT_H
#define _LOGFORMAT_H

#include<string>
#include<functional>
#include"base/Timer.h"
#include"base/Mutex.h"

namespace unet
{
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
    
    enum LogLevel
    {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL
    };
    
    class LogFormat final
    {
        public:
            explicit LogFormat();  
            LogFormat(const LogFormat&) = delete;
            LogFormat& operator=(const LogFormat&) = delete;
            LogFormat(LogFormat&&);
            LogFormat& operator=(LogFormat&&);
            ~LogFormat();
            
            const char* formatLog(pid_t,LogLevel,const char*,const char*,int);
            const char* formatLogFile(const char*,const char*,int);

        private:
            void handleSecTimerEvent();
            
        private:
            Timer u_secTimer; //每1s使用系统调用，更新一下本地的缓存
            base::MutexLock u_mutex;

            std::string u_data; //日期
            std::string u_time; //时间
            std::string u_utime; //微秒
            std::string u_formatLog; 
            std::string u_formatLogFile;
    };
}

#endif
