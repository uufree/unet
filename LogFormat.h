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

namespace unet
{
    /*
     * 具体的日志格式参照muduo提供的日志格式：
     * 日期-时间-微秒-线程-级别-正文-源文件名-行号（以空格隔开）
     * 20180522 21:20:35.125880Z 2501 INFO hello,world Poll.cc:78 
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
            
            bool operator==(const LogFormat& format){return u_utime==format.u_utime;};

            const char* formatLog(unsigned long,LogLevel,const char*,const char*,int);
            const char* formatLogFile(const char*,const char*,int);

        private:
            void updateTime();
            
        private:
            std::string u_data; //日期
            std::string u_time; //时间
            std::string u_utime; //微秒
            std::string u_formatLog; 
            std::string u_formatLogFile;
    };
}

#endif
