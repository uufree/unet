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
        void Buffer::readInSocket(int fd)
        {
            char extrabuf[65536];
            bzero(extrabuf,65536);
            struct iovec vec[2];
            vec[0].iov_base = buffer + tailindex;
            vec[0].iov_len = getFreeSize();
            vec[1].iov_base = extrabuf;
            vec[1].iov_len = 65536;

            int freesize = getFreeSize();
            int n = ::readv(fd,vec,freesize);
    
            if(n < 0)
            {
                perror("readv error\n");
            }
            else if(n < freesize && n > 0)
            {
                tailindex += n;
            }
            else
            {   
                tailindex += getFreeSize();
                int size = n - freesize;
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

        void Buffer::writeInSocket(int fd)
        {
            int n = ::write(fd,buffer+headindex,getDataSize());
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

        void Buffer::appendInBuffer(const void* message)
        {
            int n = strlen(static_cast<const char*>(message));
            if(n+10 > getFreeSize())
            {
                while(n+10 > getFreeSize())
                {
                    KBufferSize *= 2;
                    level  = KBufferSize / 2;
                    buffer = static_cast<char*>(realloc(static_cast<void*>(buffer),static_cast<size_t>(KBufferSize)));
                }
            }

            snprintf(buffer+tailindex,getFreeSize(),"\r\n%d\r\n",n);
            strcat(buffer,static_cast<const char*>(message));
            char* ch = strchr(buffer,'\0');
            tailindex = ch - buffer;
        }
    
        char* Buffer::getCompleteMessageInBuffer()
        {
            if(key == 0)
            {
                char* ch1 = strstr(buffer+headindex,"\r\n");
                ch1 += 2;
                char* ch2 = strstr(ch1,"\r\n");
                int length = ch2 - ch1;
                if(length >= 10)
                    perror("buffer length error!\n");
                ch2 += 2;
                char buf[length];
                bzero(buf,length);
                strncpy(buf,ch1,length);
                key = atoi(buf);
                ch1 -= 2;
                headindex += (ch2-ch1); 
            }

            if(key > (tailindex-headindex))
                return nullptr;
            else
            {
                char* buf = static_cast<char*>(malloc(key));
                bzero(buf,key);
                strncpy(buf,buffer+headindex,key);
                headindex += key;
                key = 0;
                 
                if(needMove())
                {
                    strcpy(buffer,buffer+headindex);
                    tailindex -= headindex;
                    headindex = 0;
                }

                return buf;
            }
        }
    }
}




































