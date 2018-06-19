/*************************************************************************
	> File Name: LogFile.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月26日 星期六 05时54分13秒
 ************************************************************************/

#ifndef _LOGFILE_H
#define _LOGFILE_H

#include<string>
#include"LogFormat.h"
#include"Alloc.h"

namespace unet
{
    /*
     * 具体的日志文件命名参照muduo提供的日志文件命名格式：
     * 进程名称.日期-时间.机器名称.进程号.log
     * argv[0].20180524-184321.hostname.2195.log
     */

    /*设计目的：
     * 使用这个类维护日志的文件信息以及对日志文件存储的策略
     * 目前使用的基本策略：
     * 只要Log的条目超过了1G，立刻换Log接着上面的写
     */

    class LogFile final
    {
        static const long MAX_FILE_SIZE = 1024 * 1024 * 1024;
        
        public:
            explicit LogFile();
            LogFile(const LogFile&) = delete;
            LogFile& operator=(const LogFile&) = delete;
            LogFile(LogFile&&);
            LogFile& operator=(LogFile&&);
            ~LogFile();
            
            bool operator==(const LogFile& log){return u_fd == log.u_fd;};

            int fd() const{return u_fd;};
            void writen(base::LogBuffer*);
            int size() const{return u_size;};
            bool isOpen() const{return u_open;};
            const std::string& logFileName() const{return u_fileName;};
            void open();
            void close();

        private:
            int u_fd;
            bool u_open;
            int u_processNum;
            long u_size;    //free size
            std::string u_filePath;
            std::string u_processName;
            std::string u_hostName;
            std::string u_fileName;
            std::string u_fullPath;
            LogFormat u_format;
    };
}

#endif
