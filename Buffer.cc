/*************************************************************************
	> File Name: Buffer.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月14日 星期二 19时28分46秒
 ************************************************************************/

#include"Buffer.h"
#include<iostream>
#include<unistd.h>

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

            int n = ::readv(fd,vec,2);
            int freesize = getFreeSize();
    
            if(n < 0)
            {
                std::cerr << "readv error!" << std::endl;
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
            int n = ::write(fd,buffer+headindex,9);
            if(n > 0)
            {
                headindex += n;
                if(needMove())
                {
                    strcpy(buffer,buffer+headindex);
                    headindex = 0;
                    tailindex -= n;
                }
            }
            else
            {
                std::cerr << "write error!" << std::endl;
            }
        }

        void Buffer::appendInBuffer(const void* message)
        {
            int n = strlen(static_cast<const char*>(message));
            if(n < getFreeSize())
            {
                memcpy(buffer+tailindex,const_cast<void*>(message),static_cast<size_t>(n));
                tailindex += n;
            }
            else
            {
                while(n > getFreeSize())
                {
                    KBufferSize *= 2;
                    level  = KBufferSize / 2;
                    buffer = static_cast<char*>(realloc(static_cast<void*>(buffer),static_cast<size_t>(KBufferSize)));
                }
                memcpy(static_cast<void*>(buffer+tailindex),const_cast<void*>(message),static_cast<size_t>(n));
                tailindex += n;
            }
        }
    
        void* Buffer::getInBuffer()
        {   
            int datasize = getDataSize();
            void* chlist = malloc(datasize);
            bzero(chlist,datasize);
            memcpy(chlist,static_cast<void*>(buffer+headindex),datasize);
            return chlist;
        }
    }
}




































