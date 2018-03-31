/*************************************************************************
	> File Name: Buffer.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年03月28日 星期三 01时28分26秒
 ************************************************************************/

#ifndef _BUFFER_H
#define _BUFFER_H

#include<string>
#include"../global.h"

namespace unet
{
    namespace base
    {
        class Buffer
        {
            public:
                virtual int readInSocket();
                virtual int writeInSocket();
                virtual void appendInBuffer(const char* message);
                virtual void appendInBuffer(const std::string& message);
                virtual void getInBuffer(char message[],size_t size);
                virtual void getInBuffer(std::string& message);
                virtual void sendFile(const std::string& filename);
                virtual void sendFile(const char* filename,size_t namelen);
                virtual void recvFile(const char* filename,size_t namelen);
                virtual void recvFile(std::string& file);
        };
        
        class StringBuffer : public Buffer
        {
            public:
                explicit StringBuffer(int fd,bool block) noexcept :
                    _confd(fd),
                    _buffer(),
                    _block(block)
                {};
                StringBuffer(const StringBuffer& buffer) = delete;
                StringBuffer(StringBuffer&& buffer) = delete;
                StringBuffer& operator=(const StringBuffer& buffer) = delete;
                StringBuffer& operator=(StringBuffer&& buffer) = delete;
                ~StringBuffer() noexcept{};
                
                bool operator==(const StringBuffer& buffer) {return _confd == buffer._confd;};

                int readInSocket() override
                {
                    char extraBuffer[65536];
                    int nbytes = 0;
                    if(!_block)
                    {
                        while(1)
                        {
                            int nbytes = ::read(_confd,extraBuffer,65536); 
                            if(nbytes < 0)
                            {
                                if(errno==EAGAIN || errno==EWOULDBLOCK)
                                    break;
                                else if(nbytes > 0)
                                {
                                    extraBuffer[nbytes] = EOF;
                                    appendInBuffer(extraBuffer);
                                }
                                else
                                {};
                            }
                        }
                    }
                    else
                        nbytes = readn(_confd,extraBuffer,65536);
                    return nbytes;
                }

                int writeInSocket() override
                {
                    int n = ::write(_confd,_buffer.c_str(),_buffer.size());
                    if(n > 0)
                        _buffer.erase(0,n);
                    else if(n < 0)
                        handleError(errno);
                    else
                    {};
                    return n;
                }
                
                void appendInBuffer(const char* message) override
                {
                    _buffer.append(message);
                    _buffer.append("\r\n");
                }

                void appendInBuffer(const std::string& message) override
                {
                    _buffer.append(message);
                    _buffer.append("\r\n");
                }

                void getInBuffer(std::string& message) override
                {
                    size_t index = _buffer.find_first_of("\r\n");
                    message.clear();

                    if(index != std::string::npos)
                    {
                        message.append(_buffer,0,index); 
                        _buffer.erase(0,index+2);
                    }
                }

                void sendFile(const char* filename,size_t namelen) override;
                void sendFile(const std::string& filename) override;
                void recvFile(const char* filename,size_t namelen) override;
                void recvFile(std::string& file) override;

            private:
                const int _confd;
                std::string _buffer;
                bool _block;
        };
/*        
        class VectorBuffer : public Buffer
        {
            public:
                explicit VectorBuffer(int fd) noexcept;
                VectorBuffer(const VectorBuffer& buffer) = delete;
                VectorBuffer(VectorBuffer&& buffer);
                VectorBuffer& operator=(const VectorBuffer& buffer) = delete;
                VectorBuffer& operator=(VectorBuffer&& buffer);
                ~VectorBuffer() noexcept;
                
                bool operator==(const VectorBuffer& buffer) {return _confd == buffer._confd;};

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

        class DequeBuffer : public Buffer
        {
            public:
                explicit DequeBuffer(int fd) noexcept;
                DequeBuffer(const DequeBuffer& buffer) = delete;
                DequeBuffer(DequeBuffer&& buffer);
                DequeBuffer& operator=(const DequeBuffer& buffer) = delete;
                DequeBuffer& operator=(DequeBuffer&& buffer);
                ~DequeBuffer() noexcept;
                
                bool operator==(const DequeBuffer& buffer) {return _confd == buffer._confd;};

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

        class ListBuffer : public Buffer
        {
            public:
                explicit ListBuffer(int fd) noexcept;
                ListBuffer(const ListBuffer& buffer) = delete;
                ListBuffer(ListBuffer&& buffer);
                ListBuffer& operator=(const ListBuffer& buffer) = delete;
                ListBuffer& operator=(ListBuffer&& buffer);
                ~ListBuffer() noexcept;
                
                bool operator==(const ListBuffer& buffer) {return _confd == buffer._confd;};

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
*/    
    }
}


#endif
