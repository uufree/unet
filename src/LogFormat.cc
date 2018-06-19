/*************************************************************************
	> File Name: LogFormat.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月26日 星期六 06时02分18秒
 ************************************************************************/

#include"../include/LogFormat.h"
#include"../include/Global.h"
#include<sys/time.h>

namespace unet
{
    //LogFormat
    LogFormat::LogFormat() :
        u_data(),
        u_time(),
        u_utime()
    {
        time_t now;
        if(::time(&now) < 0)
        {
            perror("::time error!\n");
            unet::handleError(errno);
        }
        updateTime();
    };

    LogFormat::LogFormat(LogFormat&& format) :
        u_data(std::move(format.u_data)),
        u_time(std::move(format.u_time)),
        u_utime(std::move(format.u_utime)),
        u_formatLog(std::move(format.u_formatLog)),
        u_formatLogFile(std::move(format.u_formatLogFile))
    {}
    
    LogFormat& LogFormat::operator=(LogFormat&& format)
    {
        if(format == *this)
            return *this;
        
        u_data = std::move(format.u_data);
        u_time = std::move(format.u_time);
        u_utime = std::move(format.u_utime);
        u_formatLog = std::move(format.u_formatLog);
        u_formatLogFile = std::move(format.u_formatLogFile);

        return *this;
    }
    
    LogFormat::~LogFormat()
    {}

    void LogFormat::updateTime()
    {
        time_t now;
        if(::time(&now) < 0)
        {
            perror("::time error!\n");
            unet::handleError(errno);
        }
        struct tm* timenow = localtime(&now);
        
        u_data = std::to_string(timenow->tm_year+1900);
        if(timenow->tm_mon < 10)
            u_data += "0" + std::to_string(timenow->tm_mon+1);
        else
            u_data += std::to_string(timenow->tm_mon);
        
        if(timenow->tm_mday < 10)
            u_data += "0" + std::to_string(timenow->tm_mday);
        else
            u_data += std::to_string(timenow->tm_mday);
        
        u_time.clear();
        if(timenow->tm_hour < 10)
            u_time += "0" + std::to_string(timenow->tm_hour) + ":";
        else
            u_time += std::to_string(timenow->tm_hour) + ":";

        if(timenow->tm_min < 10)
            u_time += "0" + std::to_string(timenow->tm_min) + ":";
        else
            u_time += std::to_string(timenow->tm_min) + ":";

        if(timenow->tm_sec < 10)
            u_time += "0" + std::to_string(timenow->tm_sec) + ".";
        else
            u_time += std::to_string(timenow->tm_sec) + ".";
    }

    const char* LogFormat::formatLog(unsigned long tid,LogLevel level,const char* message,const char* filename,int line)
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
        
        updateTime();
        u_formatLog = u_data + " " + u_time + u_utime + " " + std::to_string(tid) + " " + logLevel + " " + message + " " + filename + ":" + std::to_string(line) + "\n"; 

        return u_formatLog.c_str();
    }

    const char* LogFormat::formatLogFile(const char* proname,const char* hostname,int pronum)
    {
        std::string str(proname);
        updateTime();
        u_formatLogFile = str + "." + u_data + "-" + u_time + hostname + "." + std::to_string(pronum) + ".log";
        
        return u_formatLogFile.c_str();
    }
} 

