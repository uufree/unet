/*************************************************************************
  > File Name: Buffer.h
  > Author: uuchen
  > Mail: 1319081676@qq.com
  > Created Time: 2017年03月14日 星期二 19时28分38秒
 ************************************************************************/

/* 设计理念:使用STL容器管理内存，避免使用C-Style风格，接受性能损失
 
 * 1.一个Buffer只做一样工作，读或者写。
 * 2.异步会对Buffer加锁 
 * 3.内部紧耦合，尽可能少的向外部暴露接口
 * 4.尽量节省空间（挪移），此举会带来性能损失
 * 5.根据需要使用STL自行扩展
 */

#ifndef _BUFFER_H
#define _BUFFER_H

#include<string>
#include"global.h"

namespace unet
{
    namespace base
    {
        class Buffer
        {
            public:
                virtual size_t size() const = 0;
                virtual bool empty() const = 0;
                virtual const char* data() const = 0;
                virtual int readInSocket() = 0;
                virtual int blockReadInSocket() = 0;
                virtual int writeInSocket() = 0;
                virtual void appendInBuffer(const char* message,size_t len) = 0;
                virtual void appendInBuffer(const std::string& message) = 0;
                virtual void getInBuffer(std::string& message) = 0;
                virtual void sendFile(const std::string& filename) = 0;
                virtual void sendFile(const char* filename,size_t namelen) = 0;
                virtual void recvFile(const char* filename,size_t namelen) = 0;
                virtual void recvFile(std::string& file) = 0;
        };
        
        class StringBuffer : public Buffer
        {
            public:
                explicit StringBuffer(int fd) noexcept :
                    _confd(fd),
                    _buffer()
                {};
                StringBuffer(const StringBuffer& buffer) = delete;
                StringBuffer(StringBuffer&& buffer) = delete;
                StringBuffer& operator=(const StringBuffer& buffer) = delete;
                StringBuffer& operator=(StringBuffer&& buffer) = delete;
                ~StringBuffer() noexcept{};
                
                bool operator==(const StringBuffer& buffer) {return _confd == buffer._confd;};
                
                size_t size() const override {return _buffer.size();};
                bool empty() const override {return !_buffer.size();};
                const char* data() const override{return _buffer.data();};
                int blockReadInSocket() override;
                int readInSocket() override;
                int writeInSocket() override;
                void appendInBuffer(const char* message,size_t len) override;
                void appendInBuffer(const std::string& message) override;
                void getInBuffer(std::string& message) override;

                void sendFile(const char* filename,size_t namelen) override{};
                void sendFile(const std::string& filename) override{};
                void recvFile(const char* filename,size_t namelen) override{};
                void recvFile(std::string& file) override{};

            private:
                const int _confd;
                std::string _buffer;
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

