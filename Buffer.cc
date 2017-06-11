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
#include"error.h"

namespace unet
{
    namespace net
    {
        Buffer::Buffer(int fd_,int bufferSize_) : fd(fd_),
            buffer(),
            bufferSize(bufferSize_),
            dataSize(0),
            dataIndex(0)
        {
            buffer.reserve(bufferSize);
        };

        Buffer::Buffer(Buffer&& lhs) : 
            fd(lhs.fd),
            buffer(std::move(lhs.buffer)),
            bufferSize(lhs.bufferSize),
            dataSize(lhs.dataSize),
            dataIndex(lhs.dataIndex)
        {};

        Buffer& Buffer::operator=(Buffer&& lhs)
        {
            fd = lhs.fd;
            buffer = std::move(lhs.fd);
            bufferSize = lhs.bufferSize;
            dataSize = lhs.dataSize;
            dataIndex = lhs.dataIndex;

            return *this;
        }
        
        Buffer::~Buffer()
        {};

        void Buffer::swap(Buffer& lhs)
        {
            std::swap(fd,lhs.fd);
            std::swap(buffer,lhs.buffer);
            std::swap(bufferSize,lhs.bufferSize);
            std::swap(dataSize,lhs.dataSize);
            std::swap(dataIndex,lhs.dataIndex);
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
                if(getFreeSize() >= n)
                    buffer.append(extraBuffer);
                else
                {
                    bufferSize *= 2;
                    buffer.reserve(bufferSize);
                }

                dataSize += n;
            }
            else if(n == 0)
            {}
            else
                unet::handleError(errno);
            
            return n;
        };

        int Buffer::writeInSocket()
        {
            int n = ::write(fd,buffer.c_str(),buffer.size()); 
            
            if(n > 0)
            {
                dataSize -= n;
                dataIndex += n;

                if(dataIndex >= bufferSize/2)
                {   
                    bufferSize /= 2;

                    std::string str;
                    str.reserve(bufferSize);
                    str.insert(0,buffer,dataIndex,dataSize);
                    buffer.swap(str);
                }
            }
            else if(n == 0)
            {}
            else
                unet::handleError(errno);

            return n;
        }
        
        void Buffer::appendInBuffer(const char* message)
        {
            int size = strlen(message);
            
            if(getFreeSize() >= size)
            {

            }
                
        }

    }
}
