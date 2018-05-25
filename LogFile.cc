/*************************************************************************
	> File Name: LogFile.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月26日 星期六 05时55分16秒
 ************************************************************************/

#include"LogFile.h"
#include"Alloc.h"

namespace unet
{
    //LogFile
    LogFile::LogFile() :
        u_fd(-1),
        u_open(false),
        u_processNum(unet::pid()),
        u_size(MAX_FILE_SIZE),
        u_filePath(),
        u_processName(),
        u_hostName(),
        u_fileName(),
        u_fullPath(),
        u_format()
    {
        u_filePath = ::getenv("PWD"); 

        /*注意在main函数中，使用::putenv(name=argv[0])填充NAME*/
        u_processName = ::getenv("NAME");
        
        char temp[64];
        memset(temp,'\0',64);
        u_hostName = ::gethostname(temp,64);
        u_hostName = temp;
        u_fileName = u_format.formatLogFile(u_processName.c_str(),u_hostName.c_str(),u_processNum);
        u_fullPath = u_filePath + "/" + u_fileName;

        u_fd = ::open(u_fullPath.c_str(),O_RDWR|O_CREAT|O_APPEND);
        if(u_fd < 0)
        {
            perror("open log file error!\n");
            unet::handleError(errno);
        }
        u_open = true;
    }

    LogFile::LogFile(LogFile&& log) :
        u_fd(::dup(log.u_fd)),
        u_open(false),
        u_processNum(std::move(log.u_processNum)),
        u_size(log.u_size),
        u_filePath(std::move(log.u_filePath)),
        u_processName(std::move(log.u_processName)),
        u_hostName(std::move(log.u_hostName)),
        u_fileName(std::move(log.u_fileName)),
        u_fullPath(std::move(log.u_fullPath)),
        u_format(std::move(log.u_format))
    {
        if(u_fd)
            u_open = true;
        else
        {
            perror("dup log file error!\n");
            unet::handleError(errno);
        }
    };

    LogFile& LogFile::operator=(LogFile&& log)
    {
        if(log == *this)
            return *this;
        ::close(u_fd);
        u_fd = ::dup(log.u_fd);
        
        if(u_fd)
            u_open = true;
        else
        {
            perror("dup log file error!\n");
            unet::handleError(errno);
        }
        
        u_size = log.u_size;
        u_processNum = log.u_processNum;
        u_filePath = std::move(log.u_filePath);
        u_processName = std::move(log.u_processName);
        u_hostName = std::move(log.u_hostName);
        u_fileName = std::move(log.u_fileName);
        u_fullPath = std::move(log.u_fullPath);
        u_format = std::move(log.u_format);

        return *this;
    }
    
    LogFile::~LogFile()
    {
        if(u_open)
            ::close(u_fd);
    }
    
    void LogFile::writen(LogBuffer* buffer)
    {
        int size = buffer->u_data - buffer->u_buf;
        int ws = unet::writen(u_fd,buffer->u_buf,size);
        
        if(ws < 0)
        {
            perror("log write error!");
            unet::handleError(errno);
        }
    
        u_size -= size;
        if(u_size > 0)
            return;
        
        ::close(u_fd);
        u_open = false;
        u_fileName = u_format.formatLogFile(u_processName.c_str(),u_hostName.c_str(),u_processNum);
        u_fullPath = u_filePath + "/" + u_fileName;
        u_fd = ::open(u_fullPath.c_str(),O_RDWR|O_CREAT|O_APPEND);
        if(u_fd < 0)
        {
            perror("open log file error!\n");
            unet::handleError(errno);
        }
        u_open = true;
        u_size = MAX_FILE_SIZE;
    }
}


