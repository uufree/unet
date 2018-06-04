/*************************************************************************
	> File Name: Log.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月20日 星期日 19时35分30秒
 ************************************************************************/

#ifndef _LOG_H
#define _LOG_H

#include<memory>
#include<queue>
#include<string>
#include<list>
#include<vector>

#include"LogFormat.h"
#include"LogBuffer.h"

/*2018.06.04 测试完成*/
/*这个Log不走寻常路，有利有弊吧*/
namespace unet
{
    /*
     * 整个Log由以下几部分组成：
     *  LogFile：负责LogFile的滚动,以及将LogBuffer写入到具体的Log文件中
     *  LogFormat：格式化字符串，缓存一些时间数据，避免重复的系统调用
     *  LogBufferQueue：LogBuffer实体，缓存写入的Log
     *  Log：外部接口，可以调整Log等级
     */
    static const int HASH = 5;

    class Log final
    {
        typedef std::vector<LogBufferQueue*> HashBuckets;

        public:
            explicit Log();
            Log(const Log&) = delete;
            Log& operator=(const Log&) = delete;
            Log& operator=(Log&&);
            Log(Log&&);
            ~Log();
            
            bool operator==(const Log& log){return u_buckets.begin() == log.u_buckets.begin() && u_buckets.size() == log.u_buckets.size();};

            void setLogLevel(LogLevel level){u_logLevel = level;};
            LogLevel level() const{return u_logLevel;};
            void start();
            void stop();
            bool isStart() const{return u_start;};
            void log(unsigned long,const char*,const char*,int,LogLevel);
            int size() const{return u_buckets.size();};

        private:
            bool u_start;
            LogLevel u_logLevel;
            LogFormat u_format; /*格式化Log格式*/
            HashBuckets u_buckets;  /*为了细化工作线程写日志的锁粒度*/
    };
    
        static Log LOG; 
    /*注意，将来在这块填充TcpServer的全局静态Log*/
        void Logg(unsigned long,const char*,const char*,int,LogLevel);
        void LogStart();
        void LogStop();
        void SetLogLevel(LogLevel);

#define LOG_TRACE(tid,str) Logg(tid,str,__FILE__,__LINE__,TRACE)
#define LOG_DEBUG(tid,str) Logg(tid,str,__FILE__,__LINE__,DEBUG)
#define LOG_INFO(tid,str) Logg(tid,str,__FILE__,__LINE__,INFO)
#define LOG_WARN(tid,str) Logg(tid,str,__FILE__,__LINE__,WARN)
#define LOG_ERROR(tid,str) Logg(tid,str,__FILE__,__LINE__,ERROR)
#define LOG_FATAL(tid,str) Logg(tid,str,__FILE__,__LINE__,FATAL)

}

#endif
