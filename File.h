/*************************************************************************
	> File Name: File.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年02月27日 星期一 18时04分42秒
 ************************************************************************/

#ifndef _FILE_H
#define _FILE_H

//这个类的作用是封装文件描述符的操作，拒绝使用C++提供的stringstream

#include<assert.h>
#include<errno.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<string>
#include<string.h>
#include<dirent.h>
#include<iostream>

namespace unet
{
    class File final
    {
        public:
            explicit File(const char* filename_);
            File(const File&) = delete;
            File& operator=(const File&) = delete;
            ~File()
            {
                assert(!closed);
                assert(::close(fd) == 0);
            }

            void readn(void* buf,size_t nbytes);
            
            int getReadSize() const
            {return readsize;};
            

            void writen(void* buf,size_t nbytes);
    
            int getWriteSize() const
            {return writesize;};

            const std::string& getFilename() const 
            {return filename;}

            const int getFd() const
            {return fd;}

        private:
            int fd;
            std::string filename;
            bool opened,closed;
            int readsize,writesize;
    };
}
            
#endif



