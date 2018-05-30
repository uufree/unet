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

namespace unet
{
    namespace base
    {   
        static const int USR_BUFFER_INIT_LENGTH = 4;
        static const int USR_BUFFER_FULL = USR_BUFFER_INIT_LENGTH*USR_BUF_SIZE; 

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

                size_t readFreeSize() const{return u_readFreeSize;};
                size_t writeFreeSize() const{return u_writeFreeSize;};
                bool readEmpty() const{return u_readFreeSize == 0;};
                bool writeEmpty() const{return u_writeFreeSize == 0;};

                /*Functionality:
                 *      从Socket中读取数据，存在阻塞与非阻塞两个版本
                 *Parameters:
                 *      None
                 *Returned Value：
                 *      返回成功读取的字节数，否则返回-1
                 */
                int readInSocket();
                int writeInSocket();
                int readInBuffer(char* buf,size_t size);
                int writeInBuffer(const char* str,size_t size);

                void sendFile(const char* filename,size_t namelen);
                void recvFile(const char* filename,size_t namelen);

            private:
                bool u_block;
                int u_fd;
                size_t u_readFreeSize;  
                size_t u_writeFreeSize; 
                
                /*ReadList有两个任务：
                 * 1.从socket中读取数据
                 * 2.向用户提供数据
                 *为了处理数据时避免重复的计算，我们提供了必要的缓存：
                 * 从socket读取数据：
                 *  a.读取数据时，在list中可以直接开始的index
                 *  b.读取数据时，需要知道的free node数量
                 * 向用户提供数据：
                 *  a.提供数据必然从begin()开始，但是UsrBuffer设计的有问题。所以
                 *  需要一个可以起始读取数据的char*。考虑到了可以一次性没有读完
                 *  一个完整的Buffer
                 *  b.提供数据时，需要一个已存在数据的used node
                 */
                std::list<UsrBuffer*> u_readList;
                std::list<UsrBuffer*>::iterator u_readListIndex;
                int u_readListFreeLength;/*从socket读*/
                char* u_readStart;
                int u_readListUsedLength;/*用户读*/
                
                /*WriteList有两个任务：
                 * 1.buffer向socket中写数据
                 * 2.用户向buffer中写数据
                 *为了避免重复的计算，我们需要以下功能性缓存：
                 * 向buffer中写数据：
                 *  a.可以直接开始写的list index
                 *  b.能写的node的数量
                 * 向socket中写数据时：
                 *  a.首个可写buffer中的起始位置，char*，考虑一次没法写完一整个
                 *  buffer的情况
                 *  b.可以写的node数量
                 */ 
                std::list<UsrBuffer*> u_writeList;
                std::list<UsrBuffer*>::iterator u_writeListIndex;
                int u_writeListFreeLength;/*用户向buffer中写数据*/
                char* u_writeStart;
                int u_writeListUsedLength;/*buffer向socket中写数据*/
        };
    }
}
