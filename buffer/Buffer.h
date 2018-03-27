/*************************************************************************
	> File Name: Buffer.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年03月28日 星期三 01时28分26秒
 ************************************************************************/

#ifndef _BUFFER_H
#define _BUFFER_H

#include<string>

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
                virtual void recvFile(std::string& file) = 0;
        };
        
        class StringBuffer : public Buffer
        {
            public:
                explicit StringBuffer(int fd) noexcept;
                StringBuffer(const StringBuffer& buffer) = delete;
                StringBuffer(StringBuffer&& buffer);
                StringBuffer& operator=(const StringBuffer& buffer) = delete;
                StringBuffer& operator=(StringBuffer&& buffer);
                ~StringBuffer() noexcept;
                
                int readInSocket() override;
                int writeInSocket() override;
                void appendInBuffer(const char* message,size_t size) override;
                void appendInBuffer(const std::string& message) override;
                void getInBuffer(char message[],size_t size) override;
                void getInBuffer(std::string& message) override;
                void sendFile(const char* filename,size_t namelen) override;
                void sendFile(const std::string& filename) override;
                void recvFile(const char* filename,size_t namelen) override;
                void recvFile(std::string& file) override;

            private:
                int _confd;
                std::string _buffer;
        };

    }
}


#endif
