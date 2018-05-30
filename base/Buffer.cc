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
            u_writeListIndex = u_writeList.begin();
            u_readListFreeLength = USR_BUFFER_INIT_LENGTH;
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

            u_readListIndex = u_readList.begin();
            u_writeListIndex = u_writeList.begin();
            u_readListFreeLength = buf.u_readListFreeLength;
            u_writeListUsedLength = buf.u_writeListUsedLength;
        };

        Buffer& Buffer::operator=(Buffer&& buf)
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

            u_block = buf.u_block;
            u_fd = buf.u_fd;
            u_readFreeSize = buf.u_readFreeSize;
            u_writeFreeSize = buf.u_writeFreeSize;
            if(!buf.u_readList.empty())
                u_readList.splice(u_readList.begin(),buf.u_readList,buf.u_readList.begin(),buf.u_readList.end());
            if(!u_writeList.empty())
                u_writeList.splice(u_writeList.begin(),buf.u_writeList,buf.u_writeList.begin(),buf.u_writeList.end());
            
            u_readListIndex = u_readList.begin();
            u_writeListIndex = u_writeList.begin();
            u_readListFreeLength = buf.u_readListFreeLength;
            u_writeListUsedLength = buf.u_writeListUsedLength;
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

        int Buffer::readInSocket()
        {
            if(u_fd < 0)
                return -1;
            if(u_readFreeSize == 0)
                return -1;
            if(u_readListFreeLength < 1)
                return -1;
            
            struct iovec iv[u_readListFreeLength];
            auto iter = u_readListIndex;
            for(int i=0;i<u_readListFreeLength;i++)
            {
                iv[i].iov_base = (*iter)->u_buf;
                iv[i].iov_len = (*iter)->u_length;
                ++iter;
            }
            
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
            for(int i=0;i<u_readListFreeLength;++i)
            {
                if(reads >= (*iter)->u_length)
                {
                    (*iter)->u_data += (*iter)->u_length;
                    reads -= (*iter)->u_length;
                }
                else
                {
                    (*iter)->u_data += reads;
                    break;
                }
                ++iter;
            }
            

            u_readListIndex = iter;
            u_readFreeSize -= readBytes;
            
            /*即使读取到0，依旧需要查看Buffer中是否还有数据*/
            if(returnValue == 0)
                return 0;
            return readBytes;
        }
        
        int Buffer::readInBuffer(char* buf,size_t size)
        {
            if(u_fd < 0)
                return -1;
            if(u_readFreeSize <= 0 )
                return -1;
            if(u_readListFreeLength < 1)
                return -1;
            
            size_t copySize = 0;
            UsrBuffer* temp = NULL;
            for(int i=0;i<USR_BUFFER_INIT_LENGTH;i++)
            {
                temp = u_readList.front();
                if(temp->u_length == USR_BUF_SIZE)
                    break;
                else if(temp->u_length == 0)
                {
                    if(size >= temp->u_length)
                    {
                        /*将一个完整的Buffer Copy给了Usr*/
                        memcpy(buf,temp->u_buf,temp->u_length);
                        copySize += temp->u_length;
                        size -= temp->u_length;
                        
                        /*调整list*/
                        u_readList.pop_front();
                        temp->u_data = temp->u_buf;
                        temp->u_length = USR_BUF_SIZE;
                        u_readList.push_back(temp);
                        ++u_readListFreeLength;
                    }
                    else    /*size < temp->u_length*/
                    {
                        /*将一个完整的Buffer Copy给Usr一部分*/
                        memcpy(buf,temp->u_buf,size);
                        copySize += size;
                        size -= size;
                    }
                }
                else
                {
                    memcpy(buf,temp->u_buf,size);
                    copySize += size;
                    size -= size;
                }
            }
            
            u_readFreeSize += copySize;
        }
        
        int Buffer::writeInSocket()
        {
            if(u_fd < 0)
                return -1;
            if(u_writeFreeSize <= 0)
                return -1;
            if(u_writeListUsedLength < 1)
                return -1;
            
            struct iovec iv[u_writeListUsedLength];
            auto iter = u_writeListIndex;
            for(int i=0;i<u_writeListUsedLength;i++)
            {
                iv[i].iov_base = (*iter)->u_buf;
                iv[i].iov_len = (*iter)->u_length;
                ++iter;
            }
            
            int writeBytes = ::writev(u_fd,iv,u_writeListUsedLength);
            UsrBuffer* temp = NULL;
            int clearBuf = 0;
            for(int i=0;i<u_writeListUsedLength;i++)
            {
                if((*u_writeList.begin())->u_length == 0)
                {
                    temp = u_writeList.front();
                    u_writeList.pop_front();
                    /*重新初始化UsrBuffer*/
                    temp->u_data = temp->u_buf;
                    temp->u_length = USR_BUF_SIZE;
                    u_writeList.push_back(temp);
                    ++clearBuf;
                }
                else
                    break;
            }
            u_writeFreeSize += writeBytes;
            u_writeListUsedLength -= clearBuf; 

            return writeBytes;
        }
        

    }
}
