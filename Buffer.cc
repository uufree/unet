/*************************************************************************
	> File Name: Buffer.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月14日 星期二 19时28分46秒
 ************************************************************************/

#include"Buffer.h"
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>

namespace unet
{
    namespace net
    {
        void Buffer::readInSocket()
        {
            char extrabuf[65536];
            bzero(extrabuf,65536);
            struct iovec vec[2];
            vec[0].iov_base = buffer + tailindex;
            vec[0].iov_len = getFreeSize();
            vec[1].iov_base = extrabuf;
            vec[1].iov_len = 65536;

//            int n = ::readv(fd,vec,getFreeSize());
            int n = ::read(fd,buffer+tailindex,getFreeSize());
            std::cout << "-------------------------" << std::endl;
            std::cout << "read size: " << n << std::endl;

            if(n < 0)
            {
                perror("readv error\n");
            }
            else if(n < getFreeSize() && n > 0)
            {
                tailindex += n;
            }
            else
            {   
                tailindex += getFreeSize();
                int size = n - getFreeSize();
                while(size > getFreeSize())
                {
                    KBufferSize *= 2;
                    level = KBufferSize / 2;
                    buffer = static_cast<char*>(realloc(static_cast<char*>(buffer),static_cast<size_t>(KBufferSize)));
                }
                strcpy(buffer+tailindex,extrabuf);
                tailindex += size;
            }
        }

        void Buffer::writeInSocket()
        {
            int n = ::write(fd,buffer+headindex,getDataSize());
            std::cout << "write n: " << n << std::endl;
            if(n > 0)
            {
                headindex += n;
                if(needMove())
                {
                    strcpy(buffer,buffer+headindex);
                    tailindex -= headindex;
                    headindex = 0;
                }
            }
            else
            {
                perror("write error!\n");
            }
        }

        void Buffer::appendInBuffer(const char* message)
        {
            int n = strlen(message);
            std::cout << "appendInBuffer n: " << n << std::endl;
            if(n+2 > getFreeSize())
            {
                while(n+2 > getFreeSize())
                {
                    KBufferSize *= 2;
                    level  = KBufferSize / 2;
                    buffer = static_cast<char*>(realloc(static_cast<char*>(buffer),static_cast<size_t>(KBufferSize)));
                }
            }
            
            strcat(buffer,message);
            strcat(buffer,"\r\n");
            tailindex += strlen(message);
            tailindex += 2;
        }
    
        char* Buffer::getCompleteMessageInBuffer()
        {
            char* ch = strstr(buffer+headindex,"\r\n");
            if(ch != NULL)
            {
                int size = ch - buffer + headindex;
                char* buf = static_cast<char*>(malloc(size));
                strncpy(buf,buffer+headindex,size);
                headindex += size;
                headindex += 2;
                std::cout << "get n: " << size << std::endl;

                if(needMove())
                {
                    strcpy(buffer,buffer+headindex);
                    tailindex -= headindex;
                    headindex = 0;
                }
                
                return buf;
            }
            else
                return nullptr;

        }
    }
}

