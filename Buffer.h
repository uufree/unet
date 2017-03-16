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

//目前版本只支持固定大小的数据传输

namespace unet
{
    namespace net
    {
        class Buffer final
        {
            public:
                Buffer() : buffer(nullptr),KBufferSize(1024),level(KBufferSize/2),headindex(0),tailindex(0)
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
                void readInSocket(int fd);
                void writeInSocket(int fd);

                void appendInBuffer(const void* message);
                void* getInBuffer();
                
                int getDataSize()
                {return tailindex - headindex;};

                int getFreeSize()
                {return KBufferSize - tailindex;};

                bool needMove()
                {return headindex >= level;};
       
                int getHeadIndex()
                {return headindex;};

                int getTailIndex()
                {return tailindex;};
        
                int getBufferSize()
                {return KBufferSize;};

            private:
                char* buffer;
                int KBufferSize;
                int level;
                int headindex;
                int tailindex;
        };        
    }
}


































#endif

