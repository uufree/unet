/*************************************************************************
  > File Name: Buffer.h
  > Author: uuchen
  > Mail: 1319081676@qq.com
  > Created Time: 2017年03月14日 星期二 19时28分38秒
 ************************************************************************/

#ifndef _BUFFER_H
#define _BUFFER_H

#include<list>
#include"Alloc.h"

#include<iostream>
/*2018.05.31 测试完成*/

namespace unet
{
    namespace base
    {
        static const int USR_BUFFER_INIT_LENGTH = 4;
        static const int USR_BUFFER_FULL = USR_BUFFER_INIT_LENGTH*USR_BUF_SIZE;
        static const int EXTRA_BUFFER_SIZE = 65536;    

        /*每个Buffer中维护两个队列：
         * 1.读队列，长度为4，可以扩充
         * 2.写队列，长度为4，可以扩充
         * Buffer与Alloc中维护的缓冲区相互配合，缓存数据
         * */
        class Buffer final
        {
            public:
                explicit Buffer(int fd);
                Buffer(const Buffer&) = delete;
                Buffer(Buffer&&);
                Buffer& operator=(const Buffer&) = delete;
                Buffer& operator=(Buffer&&);
                ~Buffer();
                
                bool operator==(const Buffer& buf){return u_fd == buf.u_fd;};

                size_t readFreeSize() const{return u_readFreeSize;};
                size_t writeFreeSize() const{return u_writeFreeSize;};
                bool readEmpty() const{return u_readFreeSize == 0;};
                bool writeEmpty() const{return u_writeFreeSize == 0;};
                void setBlock(){u_block = true;};
                void setNonBlock(){u_block = false;};
               
                /*写缓冲区中已使用的数据*/
                size_t readUsedSize() const 
                {return u_readList.size() == USR_BUFFER_INIT_LENGTH ? USR_BUFFER_FULL-u_readFreeSize : u_readList.size()*USR_BUF_SIZE-u_readFreeSize;};
                
                /*读缓冲区中已使用的数据*/
                size_t writeUsedSize() const 
                {return u_writeList.size() == USR_BUFFER_INIT_LENGTH ? USR_BUFFER_FULL-u_writeFreeSize : u_writeList.size()*USR_BUF_SIZE-u_writeFreeSize;};

                /*Functionality:
                 *      从Socket/Buffer中读取/发送数据
                 *Parameters:
                 *      None
                 *Returned Value：
                 *      返回成功读取的字节数，否则返回-1
                 */
                int readInSocket();
                int writeInSocket();
                int readInBuffer(char* buf,size_t size);
                int writeInBuffer(const char* str,size_t size);
                
//                void sendFile(const char* filename,size_t namelen);
//                void recvFile(const char* filename,size_t namelen);

            private:
                bool u_block;   /*缓冲区不维护Socket的状态，需要标记Socket的状态*/
                int u_fd;
                size_t u_readFreeSize;  
                size_t u_writeFreeSize; 
                
                /*这个版本的Buffer使用标志对Buffer进行区分
                 *ReadList有两个任务：
                 * 1.从Socket中读取数据：选取一个Not Full的Buffer，将Socket中的
                 * 数据写入，如果Buffer数量不足，立刻分配
                 * 2.向用户提供数据：从首部的Buffer中开始读取数据，必须判断首部的
                 *   Buffer时Dirty的
                 *WriteList有两个任务：
                 * 1.往Socket中写数据：从首部的Buffer开始写数据，要求首部的Buffer
                 *   必须是Dirty的
                 * 2.用户向Buffer中写数据：选取一个Not Full的Buffer，写数据  
                 */
                std::list<UsrBuffer*> u_readList;
                /*读数据时，不能保证数据一定从当前缓冲区的起始位置开始，需要暂时
                 * 的标记读数据的位置*/
                char* u_readStart;

                std::list<UsrBuffer*> u_writeList;
                /*同样，写数据时不能保证从当前缓冲区的起始位置开始写，暂时缓存起
                 * 始位置*/
                char* u_writeStart;
        };
    }
}

#endif
