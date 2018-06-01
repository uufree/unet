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
            u_readStart(NULL),
            u_writeList(),
            u_writeStart(NULL)
        {
            for(int i=0;i<USR_BUFFER_INIT_LENGTH;i++)
            {
                u_readList.push_back(alloc::allocUsrBuffer());
                u_writeList.push_back(alloc::allocUsrBuffer());
                u_readFreeSize += USR_BUF_SIZE;
                u_writeFreeSize += USR_BUF_SIZE;
            }
        }

        Buffer::Buffer(Buffer&& buf) :
            u_block(buf.u_block),
            u_fd(buf.u_fd),
            u_readFreeSize(buf.u_readFreeSize),
            u_writeFreeSize(buf.u_writeFreeSize),
            u_readList(),
            u_readStart(NULL),
            u_writeList(),
            u_writeStart(NULL)
        {
            if(!buf.u_readList.empty())
                u_readList.splice(u_readList.begin(),buf.u_readList,buf.u_readList.begin(),buf.u_readList.end());
            if(!u_writeList.empty())
                u_writeList.splice(u_writeList.begin(),buf.u_writeList,buf.u_writeList.begin(),buf.u_writeList.end());
            
            u_readStart = buf.u_readStart;
            u_writeStart = buf.u_writeStart;
            buf.u_readStart = NULL;
            buf.u_writeStart = NULL;
        }

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
            u_readStart = buf.u_readStart;
            u_writeStart = buf.u_writeStart;

            buf.u_readStart = NULL;
            buf.u_writeStart = NULL;

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

            u_readStart = u_writeStart = NULL;
        }
    
        /*从Socket中读取数据的步骤（将数据从Socket中copy到本地的Buffer）：
         * 1.从头部开始，寻找一个Not Full的Buffer（需要遍历链表）
         * 2.填充iovec
         * 3.将数据从Socket中读取iovec中
         * 4.调整buffer中的位置，并且调整Buffer的标志
         */
        /*在非阻塞版本中，一定要将数据完全读完，否则事件不容易被处理*/
        int Buffer::readInSocket()
        {
            if(u_fd < 0)
                return -1;
            if(u_readFreeSize == 0)
                return -1;
            
            /*寻找一个Not Full并且Buffer中有剩余空间的Buffer*/
            size_t index = 0;
            std::list<UsrBuffer*>::iterator iter=u_readList.begin();
            for(;index<u_readList.size();index++)
                if(!BUFFER_TEST_FULL((*iter)) && (*iter)->u_length)
                    break;                

            /*从这个Not Full的Buffer开始填充，需要获取剩余的长度*/
            std::list<UsrBuffer*>::iterator startIter = iter;
            int readLength = u_readList.size() - index;
            if(readLength <= 0)
                return -1;
            
            char extraBuffer[65536];
            ++readLength;
            
            struct iovec iv[readLength];
            size_t oldLength = 0;
            for(int i=0;i<readLength-1;i++)
            {
                oldLength += (*iter)->u_length;
                iv[i].iov_base = (*iter)->u_data;
                iv[i].iov_len = (*iter)->u_length;
                ++iter;
            }
            iv[readLength].iov_base = extraBuffer;
            iv[readLength].iov_len = 65536;

            /*根据Socket类型，从Socket中以不同的方式读取数据*/
            size_t readBytes = 0;
            int returnValue = 1;
            if(!u_block)
            {
                while(1)
                {
                    int ret = ::readv(u_fd,iv,readLength);
                    if(ret < 0)
                    {
                        if(errno == EAGAIN || errno == EWOULDBLOCK)
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
                readBytes = ::readv(u_fd,iv,readLength);
            
            /*此时的Buffer中已经存在数据，需要相应的调整Buffer的指针位置与标志*/
            /*规范一个Buffer调整的步骤：
             * 1.从list中获取一个Buffer
             * 2.调整Buffer中指针的位置
             * 3.调整Buffer的标志
             */

            /*使用了extra中的数据，需要向readlist中添加buffer */
            if(readBytes > oldLength)
            {
                int extraSize = 65536 - oldLength;
                while(extraSize > 0)
                {
                    u_readList.push_back(alloc::allocUsrBuffer());
                    extraSize -= USR_BUF_SIZE;
                }
            }

            size_t reads = readBytes;
            while(reads)
            {
                /*数据可以完全的填充到这个Buffer中*/
                if(reads >= (*startIter)->u_length)
                {
                    /*调整buffer中指针的位置*/
                    reads -= (*startIter)->u_length;
                    (*startIter)->u_data += (*startIter)->u_length;
                    (*startIter)->u_length = 0;
                    
                    /*调整buffer中的标志位*/
                    if(BUFFER_TEST_FREE((*startIter)))
                        BUFFER_DEL_FREE((*startIter));
                    if(!BUFFER_TEST_DIRTY((*startIter)))
                        BUFFER_SET_DIRTY((*startIter));
                    if(!BUFFER_TEST_FULL((*startIter)))
                        BUFFER_SET_FULL((*startIter));
                    
                    if(reads == 0)
                        break;

                    ++startIter;
                }
                else
                {
                    /*调整buffer中指针的位置*/
                    (*startIter)->u_data += reads;
                    (*startIter)->u_length = (*startIter)->u_length - reads;
                    reads -= reads;

                    /*调整buffer中的标志*/
                    if(BUFFER_TEST_FREE((*startIter)))
                        BUFFER_DEL_FREE((*startIter));
                    if(!BUFFER_TEST_DIRTY((*startIter)))
                        BUFFER_SET_DIRTY((*startIter));
                    if(BUFFER_TEST_FULL((*startIter)))
                        BUFFER_DEL_FULL((*startIter));
                    
                    break;
                }
            }
            
            u_readFreeSize -= readBytes;
            
            if(returnValue == 0)
                return 0;
            return readBytes;
        }
        
        /*将buffer中的数据copy给Usr*/
        /*会莫名其妙的多copy出来三个字节*/
        int Buffer::readInBuffer(char* bufs,size_t size)
        {
            if(u_fd < 0)
                return -1;
            if(u_readFreeSize == USR_BUFFER_FULL)
                return -1;
            if(bufs == NULL || size == 0)
                return -1;

            char* buf = bufs;
            size_t copySize = 0;
            UsrBuffer* temp = NULL;
            size_t tempSize = 0;
            
            while(size)
            {
                /*数据的copy是从readlist的首部开始的*/
                temp = u_readList.front();
                
                if(BUFFER_TEST_FREE(temp))
                    break;

                if(u_readStart)
                    tempSize = temp->u_data - u_readStart;
                else
                {
                    u_readStart = temp->u_buf;
                    tempSize = temp->u_data - temp->u_buf;
                }
                
                /*以为着当前buffer中已经没有数据可读，可以完全被清理*/
                /*无需重新添加到队尾*/
                if(temp->u_length == USR_BUF_SIZE || tempSize <= 0)
                {
                    temp->u_length = USR_BUF_SIZE;
                    temp->u_data = temp->u_buf;
                    u_readStart = NULL;

                    if(BUFFER_TEST_FREE(temp))
                        BUFFER_SET_FREE(temp);
                    if(BUFFER_TEST_DIRTY(temp))
                        BUFFER_DEL_DIRTY(temp);
                    if(BUFFER_TEST_FULL(temp))
                        BUFFER_DEL_FULL(temp);

                    break;
                }

                if(size >= tempSize)
                {
                    /*将一个完整的Buffer copy给Usr*/
                    memcpy(buf,u_readStart,tempSize);
                    buf += tempSize;
                    copySize += tempSize;
                    size -= tempSize;

                    /*调整这个Buffer中指针的位置*/
                    u_readList.pop_front();
                    temp->u_data = temp->u_buf;
                    temp->u_length = USR_BUF_SIZE;
                    u_readList.push_back(temp);
                    u_readStart = NULL;
                    
                    /*调整Buffer中的标志*/
                    if(!BUFFER_TEST_FREE(temp))
                        BUFFER_SET_FREE(temp);
                    if(BUFFER_TEST_DIRTY(temp))
                        BUFFER_DEL_DIRTY(temp);
                    if(BUFFER_TEST_FULL(temp))
                        BUFFER_DEL_FULL(temp);

                    if(size == 0)
                        break;
                }
                else
                {   /*buffer中存在的数据量大于想要读取的数据量*/
                    memcpy(buf,u_readStart,size);
                    buf += size;
                    copySize += size;
                    u_readStart = temp->u_buf + size;
                    /*需要调整length的长度吗？*/
                    temp->u_length += size;
                    size -= size;
                    
                    if(BUFFER_TEST_FREE(temp))
                        BUFFER_DEL_FREE(temp);
                    if(!BUFFER_TEST_DIRTY(temp))
                        BUFFER_SET_DIRTY(temp);
                    if(BUFFER_TEST_FULL(temp))
                        BUFFER_DEL_FULL(temp);
                    
                    break;
                }
            }
            
            u_readFreeSize += copySize;
            return copySize;
        }

        int Buffer::writeInSocket()
        {
            if(u_fd < 0)
                return -1;
            if(u_writeFreeSize == USR_BUF_SIZE)
                return -1;
           
            /*找到第一个Not Full的Buffer*/
            int writeLength = 0;
            for(auto iter=u_writeList.begin();iter!=u_writeList.end();++iter)
                if(BUFFER_TEST_DIRTY((*iter)))
                    ++writeLength;
            
            /*根据目前Buffer中的标志位，获取可以输出的Socket长度*/
            struct iovec iv[writeLength];
            auto iter=u_writeList.begin();
            for(int i=0;i<writeLength;i++)
            {
                iv[i].iov_base = (*iter)->u_buf;
                iv[i].iov_len = (*iter)->u_data - (*iter)->u_buf;
                ++iter;
            }

            if(u_writeStart)
            {
                iv[0].iov_base = u_writeStart;
                iv[0].iov_len = (*u_writeList.begin())->u_data - u_writeStart;
            }
            
            /*向Socket中写数据*/
            int writeBytes = ::writev(u_fd,iv,writeLength);
            if(writeBytes < 0)
                return -1;
            
            /*数据已经写入完成，需要根据写入的数据量调整buffer的位置*/
            UsrBuffer* temp = NULL;
            int writes = writeBytes;
            int tempSize = 0;
            for(int i=0;i<writeLength;i++)
            {
                temp = u_writeList.front();
                if(u_writeStart)
                    tempSize = temp->u_data - u_writeStart;
                else
                {
                    u_writeStart = temp->u_buf;
                    tempSize = temp->u_data - temp->u_buf;
                }
                
                /*如果这个Buffer中的数据已经全部发送*/
                if(writeBytes >= tempSize)
                {
                    u_writeList.pop_front();
                    temp->u_data = temp->u_buf;
                    temp->u_length = USR_BUF_SIZE;
                    u_writeList.push_back(temp);
                    writeBytes -= tempSize;
                    u_writeStart = NULL;
                    
                    if(!BUFFER_TEST_FREE(temp))
                        BUFFER_SET_FREE(temp);
                    if(BUFFER_TEST_DIRTY(temp))
                        BUFFER_DEL_DIRTY(temp);
                    if(BUFFER_TEST_FULL(temp))
                        BUFFER_DEL_FULL(temp);

                    if(writeBytes == 0)
                        break;
                }       
                else
                {
                    /*这个Buffer中的一部分被发送*/
                    u_writeStart = temp->u_buf + writeBytes;
                    temp->u_length += writeBytes;
                    writeBytes -= writeBytes;

                    if(BUFFER_TEST_FREE(temp))
                        BUFFER_DEL_FREE(temp);
                    if(!BUFFER_TEST_DIRTY(temp))
                        BUFFER_SET_DIRTY(temp);
                    if(BUFFER_TEST_FULL(temp))
                        BUFFER_DEL_FULL(temp);
                }
            }
            
            u_writeFreeSize += writes;
            return writes;
        }

        int Buffer::writeInBuffer(const char* str,size_t size)
        {
            if(str == NULL || size == 0)
                return -1;
            if(u_fd  < 0)
                return -1;
            if(u_writeFreeSize == 0)
                return -1;
            
            /*在list中寻找一个Free或者Dirty&！Full的buffer*/
            std::list<UsrBuffer*>::iterator iter = u_writeList.begin();
            for(;iter!=u_writeList.end();++iter)
                if((!BUFFER_TEST_FULL((*iter)) && BUFFER_TEST_DIRTY((*iter)))|| 
                            BUFFER_TEST_FREE((*iter)))
                        break;
            
            int writeSize = 0;
            size_t tempSize = 0;
            while(size)
            {  
                /*为了防止write中没有足够的空间,重新分配*/
                if(iter == u_writeList.end())
                {
                    u_writeList.push_back(alloc::allocUsrBuffer());
                    iter = u_writeList.end();
                    --iter;
                }

                /*获取当前Buffer中的可用空间*/
                tempSize = (*iter)->u_length;
                    
                /*当前的Buffer可以被写满*/
                if(size >= tempSize)
                {
                    memcpy((*iter)->u_data,str,tempSize);
                    str += tempSize;
                    writeSize += tempSize;
                    size -= tempSize;
                    (*iter)->u_data += tempSize;
                    (*iter)->u_length = 0;

                    if(BUFFER_TEST_FREE((*iter)))
                        BUFFER_DEL_FREE((*iter));
                    if(!BUFFER_TEST_FULL((*iter)))
                        BUFFER_SET_FULL((*iter));
                    if(!BUFFER_TEST_DIRTY((*iter)))
                        BUFFER_SET_DIRTY((*iter));
                    
                    ++iter;
                    if(size == 0)
                        break;
                }
                else
                {
                    memcpy((*iter)->u_data,str,size);
                    (*iter)->u_data = (*iter)->u_buf + size;
                    (*iter)->u_length = USR_BUF_SIZE - size;
                    writeSize += size;
                    size -= size;
                    
                    if(BUFFER_TEST_FREE((*iter)))
                        BUFFER_DEL_FREE((*iter));
                    if(BUFFER_TEST_FULL((*iter)))
                        BUFFER_DEL_FULL((*iter));
                    if(!BUFFER_TEST_DIRTY((*iter)))
                        BUFFER_SET_DIRTY((*iter));

                    break;
                }
            }   

            int dirty = 0;
            for(auto iter=u_writeList.begin();iter!=u_writeList.end();++iter)
            {
                if(BUFFER_TEST_DIRTY((*iter)))
                    ++dirty;
                else
                    break;
            }

            u_writeFreeSize -= writeSize;
            return writeSize;
        }
    }
}

