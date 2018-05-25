/*************************************************************************
	> File Name: LogFormat.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月26日 星期六 06时02分18秒
 ************************************************************************/

#include"LogFormat.h"
#include"base/global.h"
#include<sys/time.h>

namespace unet
{
    //LogFormat
    LogFormat::LogFormat() :
        u_secTimer(true,1),
        u_mutex(),
        u_data(),
        u_time(),
        u_utime()
    {
        u_secTimer.setTimeCallBack(std::bind(&LogFormat::handleSecTimerEvent,this));
        
        time_t now;
        if(::time(&now) < 0)
        {
            perror("::time error!\n");
            unet::handleError(errno);
        }
        struct tm* timenow = localtime(&now);
        u_data = std::to_string(timenow->tm_year+1900)+std::to_string(timenow->tm_mon+1)+std::to_string(timenow->tm_mday);
        u_time = std::to_string(timenow->tm_hour) + ":" + std::to_string(timenow->tm_min) + ":" + std::to_string(timenow->tm_sec) + ".";

        u_secTimer.start();
    };

    LogFormat::LogFormat(LogFormat&& format) :
        u_secTimer(true,1),
        u_mutex()
    {
        {
            base::MutexLockGuard guard(format.u_mutex);
            u_data = std::move(format.u_data);
            u_time = std::move(format.u_time);
            u_utime = std::move(format.u_utime);
        }
        u_secTimer.setTimeCallBack(std::bind(&LogFormat::handleSecTimerEvent,this));
        u_secTimer.start();
    }
    
    LogFormat& LogFormat::operator=(LogFormat&& format)
    {
        format.u_secTimer.stop();
        u_secTimer.stop();
        
        {
            base::MutexLockGuard guard(format.u_mutex);
            {
                base::MutexLockGuard guard(u_mutex);
                u_data = std::move(format.u_data);
                u_time = std::move(format.u_time);
                u_utime = std::move(format.u_utime);
            }
        }
        u_secTimer.start();

        return *this;
    }
    
    LogFormat::~LogFormat()
    {
        u_secTimer.stop(); 
    }

    void LogFormat::handleSecTimerEvent()
    {
        time_t now;
        if(::time(&now) < 0)
        {
            perror("::time error!\n");
            unet::handleError(errno);
        }
        struct tm* timenow = localtime(&now);
        std::string data = std::to_string(timenow->tm_year+1900)+std::to_string(timenow->tm_mon+1)+std::to_string(timenow->tm_mday);
        std::string time = std::to_string(timenow->tm_hour) + ":" + std::to_string(timenow->tm_min) + ":" + std::to_string(timenow->tm_sec) + ".";
        
        {
            base::MutexLockGuard guard(u_mutex);
            std::swap(data,u_data);
            std::swap(time,u_time);
        }
    }

    const char* LogFormat::formatLog(pid_t pid,LogLevel level,const char* message,const char* filename,int line)
    {
        struct timeval val;
        ::gettimeofday(&val,NULL);
        u_utime = std::to_string(val.tv_usec) + "Z";
        
        std::string logLevel;
        if(level == TRACE)
            logLevel = "TRACE";
        else if(level == DEBUG)
            logLevel = "DEBUG";
        else if(level == INFO)
            logLevel = "INFO";
        else if(level == WARN)
            logLevel = "WARN";
        else if(level == ERROR)
            logLevel = "ERROR";
        else if(level == FATAL)
            logLevel = "FATAL";
        else
            logLevel = "NO LEVEL";

        {
            base::MutexLockGuard guard(u_mutex);
            u_formatLog = u_data + " " + u_time + u_utime + " " + std::to_string(pid) + " " + logLevel + " " + message + " " + filename + ":" + std::to_string(line); 
        }

        return u_formatLog.c_str();
    }

    const char* LogFormat::formatLogFile(const char* proname,const char* hostname,int pronum)
    {
        std::string str(proname);
        {
            base::MutexLockGuard guard(u_mutex);
            u_formatLogFile = str + "." + u_data + "-" + u_time + "." + hostname + "." + std::to_string(pronum) + ".log";
        }
        
        return u_formatLogFile.c_str();
    }
} 

