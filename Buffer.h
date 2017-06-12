/*************************************************************************
  > File Name: Buffer.h
  > Author: uuchen
  > Mail: 1319081676@qq.com
  > Created Time: 2017年03月14日 星期二 19时28分38秒
 ************************************************************************/

/* 设计理念:使用STL容器管理内存，避免使用C-Style风格，接受性能损失
 * 1.一个Buffer只做一样工作，读或者写。因为异步会对Buffer加锁 
 * 2.内部紧耦合，尽可能少的向外部暴露接口
 * 3.尽量节省空间（挪移），此举会带来性能损失
 * 4.根据需要使用STL自行扩展
 */

#ifndef _BUFFER_H
#define _BUFFER_H

#include<string>
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
                void getCompleteMessageInBuffer(std::string& message);
                std::string&& getCompleteMessageInBuffer();
/*                
                //针对File
                void sendFile(const char* filename);
                void sendFile(const std::string& filename); 
                void recvFile(const char* filename);
                void recvFile(const std::string& filename);
*/            
            private:
                inline int getFreeSize(int size);//得到几种不同情况的freeSize
                inline bool needToMove();//是否需要移动
                inline void handleBufferSpace(int size,const std::string& str);//处理往Buffer中写的情况
                inline void handleBufferSpace(int size);//处理从Buffer中拿出的情况
            
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

