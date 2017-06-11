/*************************************************************************
  > File Name: Buffer.h
  > Author: uuchen
  > Mail: 1319081676@qq.com
  > Created Time: 2017年03月14日 星期二 19时28分38秒
 ************************************************************************/

#ifndef _BUFFER_H
#define _BUFFER_H

#include<string>
#include<string.h>
#include<sys/uio.h>
#include<malloc.h>
#include<iostream>
#include<functional>
#include<sys/types.h>
#include<sys/stat.h>
//目前版本只支持固定大小的数据传输

namespace unet
{
    namespace net
    {
        class Buffer final
        {
            friend bool operator==(const Buffer& lhs,const Buffer& rhs);
            
            public:
                explicit Buffer(int fd_,int bufferSize = 4096);
                Buffer(const Buffer& lhs) = delete;
                Buffer(Buffer&& lhs);
                Buffer& operator=(const Buffer& lhs) = delete;
                Buffer& operator=(Buffer&& lhs);
                ~Buffer();
                
                void swap(Buffer& lhs);

                //public interface
                int readInSocket();
                int writeInSocket();
                void appendInBuffer(const char* message);
                void appendInBuffer(const std::string& message);
                void getCompleteMessageInBuffer(char* message);
                void getCompleteMessageInBuffer(std::string& message);
                std::string&& getCompleteMessageInBuffer();
                
                //针对File
                void sendFile(const char* filename);
                void sendFile(const std::string& filename); 
                void recvFile(const char* filename);
                void recvFile(const std::string& filename);
            
            private:
                int getFreeSize()
                {return bufferSize - dataSize;};

            private:
                int fd;
                std::string buffer;
                int bufferSize;
                int dataSize;
                int dataIndex;
        };        
    }
}


#endif

