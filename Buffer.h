/*************************************************************************
  > File Name: Buffer.h
  > Author: uuchen
  > Mail: 1319081676@qq.com
  > Created Time: 2017年03月14日 星期二 19时28分38秒
 ************************************************************************/

#ifndef _BUFFER_H
#define _BUFFER_H

#include<string.h>
#include<sys/uio.h>
#include<malloc.h>
#include<iostream>

//目前版本只支持固定大小的数据传输

namespace unet
{
    namespace net
    {
        class Buffer final
        {
            public:
                explicit Buffer(int fd_) : buffer(nullptr),KBufferSize(1024),level(KBufferSize/2),headindex(0),tailindex(0),key(0),fd(fd_)
            {
                buffer = static_cast<char*>(malloc(KBufferSize));
                bzero(buffer,KBufferSize);
            };

                Buffer(const Buffer& lhs) = delete;
                Buffer& operator=(const Buffer& lhs) = delete;

                ~Buffer()
                {
                    free(buffer);
                };

                //public interface
                void readInSocket();
                void writeInSocket();

                void appendInBuffer(const char* message);
                char* getCompleteMessageInBuffer();

                int getDataSize() const 
                {return tailindex - headindex;};

                void getData() const
                {
                    std::cout << "Buffer Data Size: " << tailindex-headindex << std::endl;
                    std::cout << buffer+headindex << std::endl;
                }

                int getFreeSize() const 
                {return KBufferSize - tailindex;};

                bool needMove() const
                {return headindex >= level;};

                int getHeadIndex() const 
                {return headindex;};

                int getTailIndex() const 
                {return tailindex;};

                int getBufferSize() const 
                {return KBufferSize;};
               
                bool getKey() const
                {return key;};
                
                int getFd()
                {return fd;};
            private:
                char* buffer;
                int KBufferSize;
                int level;
                int headindex;
                int tailindex;
                int key;
                int fd;
        };        
    }
}













#endif

