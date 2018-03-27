/*************************************************************************
	> File Name: Buffer.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年03月28日 星期三 01时28分26秒
 ************************************************************************/

#ifndef _BUFFER_H
#define _BUFFER_H

#include<string>
#include"../File.h"

namespace unet
{
    namespace base
    {
        class Buffer
        {
            public:
                virtual int readInSocket() = 0;
                virtual int writeInSocket() = 0;
                virtual void appendInBuffer(const char* message,size_t size) = 0;
                virtual void appendInBuffer(const std::string& message) = 0;
                virtual void getInBuffer(char message[],size_t size) = 0;
                virtual void getInBuffer(std::string& message) = 0;
                virtual void sendFile(const std::string& filename) = 0;
                virtual void sendFile(const char* filename,size_t namelen) = 0;
                virtual void recvFile(const char* filename,size_t namelen) = 0;
                virtual void recvFile(const std::string& file) = 0;
        };

    }
}


#endif
