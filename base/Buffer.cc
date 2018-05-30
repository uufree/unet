/*************************************************************************
	> File Name: Buffer.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月14日 星期二 19时28分46秒
 ************************************************************************/

#include"Buffer.h"
#include<sys/uio.h>

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
            u_readStart = u_readList.front()->u_buf;
            u_readListUsedLength = 0;

            u_writeListIndex = u_writeList.begin();
            u_writeListFreeLength = USR_BUFFER_INIT_LENGTH;
            u_writeStart = u_readList.front()->u_buf;
            u_writeListUsedLength = 0;
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
            
            /*填充向量读的数据结构*/
            struct iovec iv[u_readListFreeLength];
            auto iter = u_readListIndex;
            for(int i=0;i<u_readListFreeLength;i++)
            {
                iv[i].iov_base = (*iter)->u_buf;
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
            iter = u_readListIndex;
            size_t reads = readBytes;
            size_t usedBuf = 0;

            /*将Buffer调整好了位置*/
            for(int i = 0;i<u_readListFreeLength;++i)
            {
                if(reads > (*iter)->u_length)/*这个Buffer全部读满*/
                {
                    (*iter)->u_data += (*iter)->u_length;
                    (*iter)->u_length = 0;
                    reads -= (*iter)->u_length;
                }
                else
                {
                    (*iter)->u_data += reads;
                    (*iter)->u_length = (*iter)->u_length - reads;
                    reads -= reads;
                    break;
                }
                ++iter;
                ++usedBuf;
            }
            
            u_readFreeSize -= readBytes;
            u_readListIndex = iter;
            u_readListFreeLength = u_readListFreeLength - usedBuf;
            u_readListUsedLength = u_readListUsedLength + usedBuf;
            
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
            
            char* buf = bufs;
            size_t copySize = 0;
            UsrBuffer* temp = NULL;
            size_t tempSize = 0;
            /*遍历已经保存有数据的buffer*/
            for(int i=0;i<u_readListUsedLength;i++)
            {
                temp = u_readList.front();
                tempSize = temp->u_data - u_readStart;
                /*如果这个buffer中不存在数据，直接退出循环*/
                if(temp->u_length == USR_BUF_SIZE)/*buffer中没有数据*/
                    break;
                else if(tempSize > 0)
                {   
                    /*紧接着根据buffer中的数据量和需要读取的数据量进行判断*/
                    if(size >= tempSize)
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
                        u_readStart = u_readList.front()->u_buf;
                        ++u_readListFreeLength;
                        --u_readListUsedLength;
                    }
                    else    /*size < tempSize*/
                    {
                        /*将一个完整的Buffer Copy给Usr一部分*/
                        memcpy(buf,u_readStart,size);
                        buf += size;
                        copySize += size;
                        size -= size;

                        /*调整list*/
                        /*只是将buffer中的一部分数据copy给用户，无需大的调整*/
                        u_readStart += size;
                        break;
                    }
                }
                else/*执行到这一块，说明buffer中的数据不够用户读取*/
                    break;
            }
            
            u_readFreeSize += copySize;
            return copySize;
        }
        
        int Buffer::writeInSocket()
        {
            if(u_fd < 0)
                return -1;
            if(u_writeListUsedLength < 1)
                return -1;
            
            struct iovec iv[u_writeListUsedLength];
            auto iter = u_writeList.begin();
            for(int i=0;i<u_writeListUsedLength;i++)
            {
                iv[i].iov_base = (*iter)->u_buf;
                iv[i].iov_len = (*iter)->u_data - (*iter)->u_buf;
                ++iter;
            }
            /*为了防止上一次的buffer位置不在起始位置*/
            iv[0].iov_base = u_writeStart;
            iv[0].iov_len = (*iter)->u_data - u_writeStart;

            /*获取向buffer中写入的数据量*/ 
            int writeBytes = ::writev(u_fd,iv,u_writeListUsedLength);
            UsrBuffer* temp = NULL;
            int tempSize = 0;
            int writes = writeBytes;
            /*调整buffer list中位置指针*/
            for(int i=0;i<u_writeListUsedLength;i++)
            {
                temp = u_writeList.front();
                tempSize = temp->u_data - u_writeStart;
                
                /*这个buffer中的数据完全被处理了*/
                if(writeBytes >= tempSize)
                {
                    u_writeList.pop_front();
                    
                    /*重新初始化UsrBuffer*/
                    temp->u_data = temp->u_buf;
                    temp->u_length = USR_BUF_SIZE;
                    u_writeList.push_back(temp);
                    writeBytes -= tempSize;
                    /*调整下一个buffer的位置指针*/
                    u_writeStart = (*u_writeList.begin())->u_buf;
                    ++u_writeListFreeLength;
                    --u_writeListUsedLength;
                }
                else
                {
                    /*buffer中只有一部分数据被处理*/
                    u_writeStart += writeBytes;
                    writeBytes -= writeBytes;
                }
            }
            u_writeFreeSize += writes;

            return writes;
        }
        
        int Buffer::writeInBuffer(const char* str,size_t size)
        {
            if(str == NULL || size > u_writeFreeSize)
                return -1;
            if(u_writeListFreeLength < 1)
                return -1;
            
            size_t tempSize = 0;
            int writeSize = 0;
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
                    --u_writeListFreeLength;
                    ++u_writeListUsedLength;
                }
                else
                {
                    memcpy((*u_writeListIndex)->u_data,str,size);
                    (*u_writeListIndex)->u_data = (*u_writeListIndex)->u_buf + size;
                    (*u_writeListIndex)->u_length = USR_BUF_SIZE - size;
                    writeSize += size;
                    size -= size;
                    break;
                }
            }
            u_writeFreeSize -= writeSize;

            return writeSize;
        }
        
    }
}
