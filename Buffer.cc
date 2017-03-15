/*************************************************************************
	> File Name: Buffer.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月14日 星期二 19时28分46秒
 ************************************************************************/

namespace unet
{
    namespace net
    {
        void Buffer::readInSocket(int fd)
        {
            char extrabuf[65536];
            bzero(extrabuf,65536);
            struct iovec vec[2];
            vec[0].iov_base = buffer + tailindex;
            vev[0].iov_len = getFreeSize();
            vec[1].iov_base = extrabuf;
            vec[1].iov_len = 65536;

            int n = ::readv(fd,vec,2);
            int freesize = getFreeSize();
            
            if(n < 0)
            {
                std::cerr << "readv error!" << endl;
            }
            else if(n < freesize && n > 0)
            {
                tailindex += (n-1);
            }
            else
            {
                int size = n - freesize;
                while(size > freesize)
                {
                    KBufferSize *= 2;
                    level = KBufferSize / 2;
                    realloc(buffer,KBufferSize);
                }
                memcpy(buffer+tailindex,extrabuf,size);
                tailindex += (n-1);
            }
        }

        void Buffer::writeInSocket(int fd)
        {
            int n = ::write(fd,buffer+headindex,getDataSize());
            if(n > 0)
            {
                headindex += n;
                if(needMove())
                {
                    char* buffer_;
                    memcpy(buffer_,buffer+headindex,getDataSize());
                    memcpy(buffer,buffer_,getDataSize());
                    headindex = 0;
                    tailidex -= n;
                }
            }
            else
            {
                std::cerr << "write error!" << std::endl;
            }
        }

        void Buffer::appendInBuffer(const void* message)
        {
            int n = sizeof(message) - 1;
            if(n < getFreeSize())
            {
                memcpy(buffer+index,const_cast<void*>(message),n);
                tailindex += n;
            }
            else
            {
                while(n > getFreeSize())
                {
                    KBufferSize *= 2;
                    level  = KBufferSize / 2;
                    realloc(buffer,KBufferSize);
                }
                memcpy(buffer+tailindex,const_cast<void*>(message),n);
                tailindex += n;
            }
        }

        void* Buffer::getInBuffer()
        {   
            int datasize = getDataSize();
            char charlist[datasize];
            memcpy(charlist,buffer+headindex,datasize);
            return charlist;
        }
    }
}




































