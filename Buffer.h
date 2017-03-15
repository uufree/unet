/*************************************************************************
	> File Name: Buffer.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月14日 星期二 19时28分38秒
 ************************************************************************/

#ifndef _BUFFER_H
#define _BUFFER_H

#include<string.h>

//选取固定大小的数据传输

namespace unet
{
    namespace net
    {
        class Buffer final
        {
            public:
                Buffer(int KBufferSize_ = 1024) : index(0),KBufferSize(KBufferSize_),level(KBufferSize/2),indextohead(0)
                {
                    buffer = malloc(KBufferSize);
                    bzero(buffer);
                };

                Buffer(const Buffer& lhs) = delete;
                Buffer& Buffer(const Buffer& lhs) = delete;

                ~Buffer()
                {
                    free(buffer);
                };
//public interface
                void readInSocket(int fd);
                void writeInSocket(int fd);

                void appendInBuffer(const void* message);
                void* getInBuffer();
                
            private:
                int getDataSize()
                {return tailindex - headindex;};

                int getFreeSize()
                {return KBufferSize - tailindex;};

                bool needMove()
                {return headindex >= level;};

                char buffer[KBufferSize];
                int tailindex;
                int level;
                int KBufferSize;
                int headindex;
        };
    }
}


































#endif

