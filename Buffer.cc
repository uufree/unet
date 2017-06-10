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
#include"File.h"

namespace unet
{
    namespace net
    {
        Buffer::Buffer(int fd_,int bufferSize_) : fd(fd_),
            buffer(),
            bufferSize(bufferSize_),
            readIndex(0),
            readSize(0),
            writeIndex(0),
            writeSize(0)
        {
            buffer.reserve(bufferSize);
        };

        Buffer::Buffer(Buffer&& lhs) : 
            fd(lhs.fd),
            buffer(std::move(lhs.buffer)),
            bufferSize(lhs.bufferSize), 
            readIndex(lhs.readIndex),
            readSize(lhs.readSize),
            writeIndex(lhs.writeIndex),
            writeSize(lhs.writeSize)
        {};

        Buffer& Buffer::operator=(Buffer&& lhs)
        {
            fd = lhs.fd;
            buffer = std::move(lhs.fd);
            bufferSize = lhs.bufferSize;

            readIndex = lhs.readIndex;
            readSize = lhs.readSize;
            writeIndex = lhs.writeIndex;
            writeSize = lhs.writeSize;

            return *this;
        }
        
        Buffer::~Buffer()
        {};

        void Buffer::swap(Buffer& lhs)
        {
            std::swap(fd,lhs.fd);
            std::swap(buffer,lhs.buffer);
            std::swap(bufferSize,lhs.bufferSize);
            std::swap(readIndex,lhs.readIndex);
            std::swap(readSize,lhs.readSize);
            std::swap(writeIndex,lhs.writeIndex);
            std::swap(writeSize,lhs.writeSize);
        }
        
        bool operator==(const Buffer& lhs,const Buffer& rhs)
        {
            if(lhs.fd == rhs.fd)
                return true;
            return false;
        };

        int Buffer::readInSocket()
        {
            char extraBuffer[65535];
            bzero(extraBuffer,65535);
            int n = ::read(fd,extraBuffer,65535);
            std::cout << "read n: " << n << std::endl;

            if(n > 0)
            {
                buffer.insert()            
            }
                
        };
        

    }
}
