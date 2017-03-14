/*************************************************************************
	> File Name: Buffer.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月14日 星期二 19时28分38秒
 ************************************************************************/

#ifndef _BUFFER_H
#define _BUFFER_H

//选取固定大小的数据传输

namespace unet
{
    namespace net
    {
        class Buffer final
        {
            public:
                Buffer(int level_) : index(0),level(level_)
                {};

                Buffer(const Buffer& lhs) = delete;
                Buffer& Buffer(const Buffer& lhs) = delete;

                void readInSocket(int fd);
                void writeInSocket(int fd);

                void appendInBuffer(const void* message);
                void* getInBuffer();
                
                bool indexGreaterThanLevel()
                {return index > level;};

            private:
                std::vector<char> buffer;
                int index;
                const int level;
        };
    }
}


































#endif

