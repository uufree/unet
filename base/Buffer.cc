/*************************************************************************
	> File Name: Buffer.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月14日 星期二 19时28分46秒
 ************************************************************************/

#include"Buffer.h"
#include"Alloc.h"
#include"UsrBuffer.h"

namespace unet
{
    namespace base
    {
        Buffer::Buffer(int fd) :
            u_fd(fd),
            u_readSize(0),
            u_writeSize(0),
            u_readList(),
            u_writeList()
        {
            for(int i=0;i<USR_BUFFER_INIT_LENGTH;i++)
            {
                u_readList.push_back(alloc::allocUsrBuffer());
                u_writeList.push_back(alloc::allocUsrBuffer());
                u_readSize += USR_BUF_SIZE;
                u_writeSize += USR_BUF_SIZE;
            }
        }

        Buffer::Buffer(Buffer&& buf) :
            u_fd(buf.u_fd),
            u_readSize(buf.u_readSize),
            u_writeSize(buf.u_writeSize),
            u_readList(),
            u_writeList()
        {
            if(!buf.u_readList.empty())
                u_readList.splice(u_readList.begin(),buf.u_readList,buf.u_readList.begin(),buf.u_readList.end());
            if(!u_writeList.empty())
                u_writeList.splice(u_writeList.begin(),buf.u_writeList,buf.u_writeList.begin(),buf.u_writeList.end());
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

            u_fd = buf.u_fd;
            u_readSize = buf.u_readSize;
            u_writeSize = buf.u_writeSize;
            if(!buf.u_readList.empty())
                u_readList.splice(u_readList.begin(),buf.u_readList,buf.u_readList.begin(),buf.u_readList.end());
            if(!u_writeList.empty())
                u_writeList.splice(u_writeList.begin(),buf.u_writeList,buf.u_writeList.begin(),buf.u_writeList.end());
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
    }
}
