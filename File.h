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
#include<iostream>
//#include<functional>

namespace unet
{
    class File final
    {
        public:
//            typedef std::function<void ()> callbackFunc;
            static const int buffSize = 64 * 1024;
            explicit File(std::string& filename_,bool isoldfile=true) : filename(filename_)
            {
                if(!isoldfile)
                {//打开新文件
                    fd = ::open(filename.c_str(),O_RDWR|O_APPEND|O_CLOEXEC|O_CREAT|O_EXCL|O_TRUNC);
                }
                else
                {
                    fd = ::open(filename.c_str(),O_RDWR|O_APPEND);
                }
//                assert(fd <= 0);
                std::cout << fd << std::endl;              
                opened = true;
                closed = false;

//                readbuf = static_cast<char>(malloc(buffSize));
//                writebuf = static_cast<char>(malloc(buffSize));
//                memset(readbuf,0,buffSize);
//                memset(writebuf,0,buffSize);
            }    
                
            File(const File&) = delete;
            File(File&&) = delete;
            File& operator=(const File&) = delete;

            ~File()
            {
                assert(!closed);
                assert(::close(fd) == 0);

//                free(readbuf);
//                free(writebuf);
            }

            ssize_t readn(void* buf,size_t nbytes) const
            {
                
                int nleft,nread;
                char* cptr;

                cptr = (char*)buf;
                nleft = nbytes;

                while(nleft > 0)
                {
                    if((nread=read(fd,cptr,nleft)) < 0)
                    {
                        if(errno == EINTR)
                            nread = 0;
                        else
                            return -1;
                    }
                    else if(nread == 0)
                        break;
                    nleft -= nread;
                    cptr += nread;
                }
                return (nbytes-nleft);
            }

            

            ssize_t writen(void* buf,size_t nbytes) const
            {
                int nleft,nwriten;
                char* cptr;

                cptr = (char*)buf;
                nleft = nbytes;

                while(nleft > 0)
                {
                    if((nwriten=write(fd,cptr,nleft)) <= 0)
                    {
                        if(nwriten<0 && errno!=EINTR)
                            nwriten = 0;
                        else
                            return -1;
                    }
                    nleft -= nwriten;
                    cptr += nwriten;
                }
                return (nbytes-nleft);
            }

            const std::string& getFilename() const 
            {
                return filename;
            }

            const int getFd() const
            {
                return fd;
            }

        private:
            int fd;
//            callnackFunc readCallback;
//            callbackFunc writeCallback;
            std::string filename;
//            char readbuf[buffSize];
//            char writebuf[buffSize];
            bool opened;
            bool closed;
    };
}
            
#endif



