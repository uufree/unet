/*************************************************************************
	> File Name: Buffer.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月14日 星期二 19时28分46秒
 ************************************************************************/

#include"Buffer.h"
#include<sys/uio.h>
#include<iostream>

namespace unet
{
    namespace base
    {
        Buffer::Buffer(int fd) :
            u_block(false),
            u_fd(fd),
            u_readFreeSize(0),
            u_writeFreeSize(0),
            u_readList(),
            u_writeList()
        {
            for(int i=0;i<USR_BUFFER_INIT_LENGTH;i++)
            {
                u_readList.push_back(alloc::allocUsrBuffer());
                u_writeList.push_back(alloc::allocUsrBuffer());
                u_readFreeSize += USR_BUF_SIZE;
                u_writeFreeSize += USR_BUF_SIZE;
            }

            u_readListIndex = u_readList.begin();
            u_readListFreeLength = USR_BUFFER_INIT_LENGTH;
            u_readStart = NULL;
            u_readListUsedLength = 0;   /*初始化时确实没有数据占用buffer*/

            u_writeListIndex = u_writeList.begin();
            u_writeListFreeLength = USR_BUFFER_INIT_LENGTH;
            u_writeStart = NULL;
            u_writeListUsedLength = 0;  /*初始化时确实没有数据占用buffer*/
        }

        Buffer::Buffer(Buffer&& buf) :
            u_block(buf.u_block),
            u_fd(buf.u_fd),
            u_readFreeSize(buf.u_readFreeSize),
            u_writeFreeSize(buf.u_writeFreeSize),
            u_readList(),
            u_writeList()
        {
            if(!buf.u_readList.empty())
                u_readList.splice(u_readList.begin(),buf.u_readList,buf.u_readList.begin(),buf.u_readList.end());
            if(!u_writeList.empty())
                u_writeList.splice(u_writeList.begin(),buf.u_writeList,buf.u_writeList.begin(),buf.u_writeList.end());

            u_readListIndex = buf.u_readListIndex;
            u_readListFreeLength = buf.u_readListFreeLength;
            u_readStart = buf.u_readStart;
            u_readListUsedLength = buf.u_readListUsedLength;

            u_writeListIndex = buf.u_writeListIndex;
            u_readListFreeLength = buf.u_readListFreeLength;
            u_writeStart = buf.u_writeStart;
            u_writeListUsedLength = buf.u_writeListUsedLength;
        };

        Buffer& Buffer::operator=(Buffer&& buf)
        {
            if(buf == *this)
                return *this;

            while(!u_readList.empty())
            {
                alloc::deallocUsrBuffer(u_readList.front());
                u_readList.pop_front();
            }

            while(!u_writeList.empty())
            {
                alloc::deallocUsrBuffer(u_writeList.front());
                u_writeList.pop_front();
            }

            u_block = buf.u_block;
            u_fd = buf.u_fd;
            u_readFreeSize = buf.u_readFreeSize;
            u_writeFreeSize = buf.u_writeFreeSize;
            if(!buf.u_readList.empty())
                u_readList.splice(u_readList.begin(),buf.u_readList,buf.u_readList.begin(),buf.u_readList.end());
            if(!u_writeList.empty())
                u_writeList.splice(u_writeList.begin(),buf.u_writeList,buf.u_writeList.begin(),buf.u_writeList.end());
            
            u_readListIndex = buf.u_readListIndex;
            u_readListFreeLength = buf.u_readListFreeLength;
            u_readStart = buf.u_readStart;
            u_readListUsedLength = buf.u_readListUsedLength;

            u_writeListIndex = buf.u_writeListIndex;
            u_readListFreeLength = buf.u_readListFreeLength;
            u_writeStart = buf.u_writeStart;
            u_writeListUsedLength = buf.u_writeListUsedLength;
           
            return *this;
        }

        Buffer::~Buffer()
        {
            while(!u_readList.empty())
            {
                alloc::deallocUsrBuffer(u_readList.front());
                u_readList.pop_front();
            }

            while(!u_writeList.empty())
            {
                alloc::deallocUsrBuffer(u_writeList.front());
                u_writeList.pop_front();
            }
        }
        
        /*从socket中读取数据*/
        int Buffer::readInSocket()
        {
            if(u_fd < 0)
                return -1;
            if(u_readFreeSize == 0)
                return -1;
            if(u_readListFreeLength < 1)
                return -1;
            
            std::cout << "~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
            std::cout << "Read in socket before: " << std::endl;
            std::cout << "read list free length: " << u_readListFreeLength << std::endl;
            std::cout << "read list used length: " << u_readListUsedLength << std::endl;
            std::cout << "~~~~~~~~~~~~~~~~~~~~~~" << std::endl;

            /*填充向量读的数据结构*/
            struct iovec iv[u_readListFreeLength];
            auto iter = u_readListIndex;
            for(int i=0;i<u_readListFreeLength;i++)
            {
                iv[i].iov_base = (*iter)->u_data;
                iv[i].iov_len = (*iter)->u_length;
                ++iter;
            }
            
            /*获取本次读取的数据量*/
            size_t readBytes = 0;
            /*防止在n次读取时突然读取到0*/
            int returnValue = 1;
            if(!u_block)
            {
                while(1)
                {   
                    int ret = ::readv(u_fd,iv,u_readListFreeLength);
                    if(ret < 0)
                    {
                        if(errno ==EAGAIN || errno == EWOULDBLOCK)
                            break;
                    }
                    else if(ret == 0)
                    {
                        returnValue = 0;
                        break;
                    }
                    else 
                        readBytes += ret;
                }
            }
            else
                readBytes = ::readv(u_fd,iv,u_readListFreeLength);
            
            /*根据读取到的量调整指针位置*/
            /*需要根据读取的数据量填充used length*/
            iter = u_readListIndex; /*获取当前数据已经被填充的起始buffer*/
            size_t reads = readBytes;
            size_t usedBuf = 0;

            /*将Buffer调整好了位置*/
            for(int i = 0;i<u_readListFreeLength;i++)
            {
                /*这个buffre全部读满*/
                if(reads > (*iter)->u_length)
                {
                    /*调整length和data的位置指针*/
                    reads -= (*iter)->u_length;
                    (*iter)->u_data += (*iter)->u_length;
                    (*iter)->u_length = 0;
                    
                    ++iter;
                    ++usedBuf;
                    ++u_readListUsedLength;;
                }
                else    /*如果这个buffer中只读取了部分数据*/
                {       /*即使只读取了部分的数据，这个buffer也应该被标记已使用*/
                    (*iter)->u_data += reads;
                    (*iter)->u_length = (*iter)->u_length - reads;
                    reads -= reads;

                    ++u_readListUsedLength;
                    break;
                }
            }
            
            u_readFreeSize -= readBytes;
            u_readListIndex = iter;
            u_readListFreeLength = u_readListFreeLength - usedBuf;
/*            
            if(u_readListFreeLength == 0)
                u_readListFreeLength = 1;
            if(u_readListUsedLength == USR_BUFFER_INIT_LENGTH + 1)
                u_readListUsedLength = USR_BUFFER_INIT_LENGTH;
*/
            std::cout << "~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
            std::cout << "Read in socket after: " << std::endl;
            std::cout << "read list free length: " << u_readListFreeLength << std::endl;
            std::cout << "read list used length: " << u_readListUsedLength << std::endl;
            std::cout << "~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
           
            /*即使读取到0，依旧需要查看Buffer中是否还有数据*/
            if(returnValue == 0)
                return 0;
            return readBytes;
        }
        
        int Buffer::readInBuffer(char* bufs,size_t size)
        {
            if(u_fd < 0)
                return -1;
            if(u_readListUsedLength < 1)
                return -1;
            
            std::cout << "~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
            std::cout << "Read in buffer before: " << std::endl;
            std::cout << "read list free length: " << u_readListFreeLength << std::endl;
            std::cout << "read list used length: " << u_readListUsedLength << std::endl;
            std::cout << "~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
            
            char* buf = bufs;
            size_t copySize = 0;
            UsrBuffer* temp = NULL;
            size_t tempSize = 0;
            int clearBuf = 0; 
            /*遍历已经保存有数据的buffer*/
            for(int i=0;i<u_readListUsedLength;i++)
            {
                /*若readStart = NULL，将readStart调整为当前buffer的起始位置*/
                temp = u_readList.front();
                if(u_readStart)
                    tempSize = temp->u_data - u_readStart;
                else
                {
                    u_readStart = temp->u_buf;
                    tempSize = temp->u_data - temp->u_buf;
                }

                /*如果这个buffer中不存在数据，直接退出循环*/
                if(temp->u_length == USR_BUF_SIZE)/*buffer中没有数据*/
                    break;
                else if(tempSize > 0)
                {   
                    /*紧接着根据buffer中的数据量和需要读取的数据量进行判断*/
                    if(size > tempSize)
                    {
                        /*将一个完整的Buffer Copy给了Usr*/
                        memcpy(buf,u_readStart,tempSize);
                        buf += tempSize;
                        copySize += tempSize;
                        size -= tempSize;
                        
                        /*调整list*/
                        /*因为这个部分是完整的读完一个buffer，调整的比较全面*/
                        u_readList.pop_front();
                        temp->u_data = temp->u_buf;
                        temp->u_length = USR_BUF_SIZE;
                        u_readList.push_back(temp);
                        u_readStart = NULL;
                        u_readListIndex = u_readList.begin();
                         
                        ++u_readListFreeLength;
                        ++clearBuf; /*标记完全清理的buffer的数量*/
                    }
                    else    /*size < tempSize*/
                    {
                        /*将一个完整的Buffer Copy给Usr一部分*/
                        /*用户没有读取全部数据的情况*/
                        memcpy(buf,u_readStart,size);
                        buf += size;
                        copySize += size;
                        u_readStart = temp->u_buf + size;
                        temp->u_length += size;
                        size -= size;
                        
                        ++clearBuf;

                        /*调整list*/
                        /*只是将buffer中的一部分数据copy给用户，无需大的调整*/
                        break;
                    }
                }
                else/*执行到这一块，说明buffer中的数据不够用户读取*/
                    break;
            }
            
            u_readListUsedLength -= clearBuf;
            
            std::cout << "~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
            std::cout << "Read in buffer after: " << std::endl;
            std::cout << "read list free length: " << u_readListFreeLength << std::endl;
            std::cout << "read list used length: " << u_readListUsedLength << std::endl;
            std::cout << "~~~~~~~~~~~~~~~~~~~~~~" << std::endl;

            u_readFreeSize += copySize;
            return copySize;
        }
        
        int Buffer::writeInSocket()
        {
            if(u_fd < 0)
                return -1;
            if(u_writeListUsedLength < 1)
                return -1;
            std::cout << "~~~~~~~~~~~~~~~~~~~" << std::endl;
            std::cout << "write in socket before: " << std::endl;
            std::cout << "write list free length: " << u_writeListFreeLength << std::endl;
            std::cout << "write list used length: " << u_writeListUsedLength << std::endl;
            std::cout << "~~~~~~~~~~~~~~~~~~" << std::endl;
            struct iovec iv[u_writeListUsedLength];
            auto iter = u_writeList.begin();
            for(int i=0;i<u_writeListUsedLength;i++)
            {
                iv[i].iov_base = (*iter)->u_buf;
                iv[i].iov_len = (*iter)->u_data - (*iter)->u_buf;
                ++iter;
                
                std::cout << "iov_len: " << iv[i].iov_len << std::endl;
            }
            
            /*为了防止上一次的buffer位置不在起始位置*/
            if(u_writeStart)
            {
                iv[0].iov_base = u_writeStart;
                iv[0].iov_len = (*u_writeList.begin())->u_data - u_writeStart;
            }
            

            /*获取向buffer中写入的数据量*/ 
            int writeBytes = ::writev(u_fd,iv,u_writeListUsedLength);
            if(writeBytes < 0)
                return -1;

            UsrBuffer* temp = NULL;
            int tempSize = 0;
            int writes = writeBytes;
            /*调整buffer list中位置指针*/
            int clearBuf = 0;
            
            for(int i=0;i<u_writeListUsedLength;i++)
            {
                temp = u_writeList.front();
                
                if(u_writeStart)
                    tempSize = temp->u_data - u_writeStart;
                else
                {
                    u_writeStart = temp->u_buf;
                    tempSize = temp->u_data - temp->u_buf;
                }
                
                std::cout << "writeBytes: " << writeBytes << std::endl;
                std::cout << "tempSize: " << tempSize << std::endl;
                /*这个buffer中的数据完全被处理了*/
                if(writeBytes > tempSize)
                {
                    u_writeList.pop_front();
                    
                    /*重新初始化UsrBuffer*/
                    temp->u_data = temp->u_buf;
                    temp->u_length = USR_BUF_SIZE;
                    u_writeList.push_back(temp);
                    writeBytes -= tempSize;
                    /*调整下一个buffer的位置指针*/
                    u_writeStart = NULL;
                    u_writeListIndex = u_writeList.begin();
                    
                    /*这个Buffer被完全清理*/
                    ++u_writeListFreeLength;
                    ++clearBuf;
                }
                else
                {
                    /*buffer中只有一部分数据被处理*/
                    u_writeStart = temp->u_buf + writeBytes;;
                    temp->u_length += writeBytes;
                    writeBytes -= writeBytes;
                    
                    ++clearBuf;
                    break;
                }
            }
            
            u_writeListUsedLength -= clearBuf;
            u_writeFreeSize += writes;

            
            std::cout << "~~~~~~~~~~~~~~~~~~~" << std::endl;
            std::cout << "write in socket after: " << std::endl;
            std::cout << "write list free length: " << u_writeListFreeLength << std::endl;
            std::cout << "write list used length: " << u_writeListUsedLength << std::endl;
            std::cout << "~~~~~~~~~~~~~~~~~~" << std::endl;

            return writes;
        }
        
        int Buffer::writeInBuffer(const char* str,size_t size)
        {
            if(str == NULL || size > u_writeFreeSize)
                return -1;
            if(u_writeListFreeLength < 1)
                return -1;
            std::cout << "~~~~~~~~~~~~~~~~~~~" << std::endl;
            std::cout << "write in buffer before: " << std::endl;
            std::cout << "write list free length: " << u_writeListFreeLength << std::endl;
            std::cout << "write list used length: " << u_writeListUsedLength << std::endl;
            std::cout << "~~~~~~~~~~~~~~~~~~" << std::endl;
            
            size_t tempSize = 0;
            int writeSize = 0;
            int usedBuf = 0;
            for(int i=0;i<u_writeListFreeLength;i++)
            {
                tempSize = (*u_writeListIndex)->u_length;
                /*数据可以完整的写入这个buffer*/
                if(size > tempSize)
                {
                    memcpy((*u_writeListIndex)->u_data,str,tempSize);
                    str += tempSize;
                    writeSize += tempSize;
                    size -= tempSize;
                    (*u_writeListIndex)->u_data += tempSize;
                    (*u_writeListIndex)->u_length = 0;

                    ++u_writeListIndex;
                    ++u_writeListUsedLength;
                    ++usedBuf;
                }
                else
                {
                    memcpy((*u_writeListIndex)->u_data,str,size);
                    (*u_writeListIndex)->u_data = (*u_writeListIndex)->u_buf + size;
                    (*u_writeListIndex)->u_length = USR_BUF_SIZE - size;
                    writeSize += size;
                    size -= size;
                    
                    /*即使只写了一部分数据，依然需要将这个buffer标记为used*/
                    ++u_writeListUsedLength;

                    break;
                }
            }
            u_writeListFreeLength -= usedBuf;
            u_writeFreeSize -= writeSize;
            std::cout << "~~~~~~~~~~~~~~~~~~~" << std::endl;
            std::cout << "write in buffer after: " << std::endl;
            std::cout << "write list free length: " << u_writeListFreeLength << std::endl;
            std::cout << "write list used length: " << u_writeListUsedLength << std::endl;
            std::cout << "~~~~~~~~~~~~~~~~~~" << std::endl;

            return writeSize;
        }
        
    }
}
