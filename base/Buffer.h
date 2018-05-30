/*************************************************************************
  > File Name: Buffer.h
  > Author: uuchen
  > Mail: 1319081676@qq.com
  > Created Time: 2017年03月14日 星期二 19时28分38秒
 ************************************************************************/

#ifndef _BUFFER_H
#define _BUFFER_H

#include"Global.h"
#include<list>

namespace unet
{
    namespace base
    {   
        static const int USR_BUFFER_INIT_LENGTH = 4;
        
        class UsrBuffer;

        class Buffer final
        {
            public:
                explicit Buffer(int fd);
                Buffer(const Buffer&) = delete;
                Buffer(Buffer&&);
                Buffer& operator=(const Buffer&) = delete;
                Buffer& operator=(Buffer&&);
                ~Buffer();
                
                bool operator==(const Buffer& buf){return buf.u_fd == u_fd;};

                size_t readSize() const{return u_readSize;};
                size_t writeSize() const{return u_writeSize;};
                bool readEmpty() const{return u_readSize == 0;};
                bool writeEmpty() const{return u_writeSize == 0;};
                const char* readData() const{};
                char* data();
                int readInSocket();
                int blockReadInSocket();
                int writeInSocket();
                int blockWriteInSocket();
                int appendInBuffer(const char* str,size_t size);

                void sendFile(const char* filename,size_t namelen);
                void recvFile(const char* filename,size_t namelen);

            private:
                int u_fd;
                size_t u_readSize;
                size_t u_writeSize;
                std::list<UsrBuffer*> u_readList;
                std::list<UsrBuffer*> u_writeList;
        };
    }
}
