/*************************************************************************
	> File Name: BufferTest.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月15日 星期三 15时31分52秒
 ************************************************************************/

#include<iostream>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include<sys/uio.h>

using namespace std;

class Buffer final
{
    public:
        Buffer() : buffer(nullptr),KBufferSize(1024),level(KBufferSize/2),headindex(0),tailindex(0)
        {
            buffer = static_cast<char*>(malloc(KBufferSize));
            bzero(buffer,KBufferSize);
        };

        Buffer(const Buffer& lhs) = delete;
        Buffer& operator=(const Buffer& lhs) = delete;

        ~Buffer()
        {
            free(buffer);
        };
//public interface
        void readInSocket(int fd);
        void writeInSocket(int fd);

        void appendInBuffer(const void* message);
        void* getInBuffer();
                
        int getDataSize()
        {return tailindex - headindex;};

        int getFreeSize()
        {return KBufferSize - tailindex;};

        bool needMove()
        {return headindex >= level;};
       
        int getHeadIndex()
        {return headindex;};

        int getTailIndex()
        {return tailindex;};

    private:
        char* buffer;
        int KBufferSize;
        int level;
        int headindex;
        int tailindex;
};


void Buffer::readInSocket(int fd)
{
    char extrabuf[65536];
    bzero(extrabuf,65536);
    struct iovec vec[2];
    vec[0].iov_base = buffer + tailindex;
    vec[0].iov_len = getFreeSize();
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
        tailindex += n;
    }
    else
    {
        int size = n - freesize;
        while(size > freesize)
        {
            KBufferSize *= 2;
            level = KBufferSize / 2;
            buffer = static_cast<char*>(realloc(static_cast<char*>(buffer),static_cast<size_t>(KBufferSize)));
        }
        memcpy(buffer+tailindex,extrabuf,size);
        tailindex += n;
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
            int datasize = getDataSize();
            char* buffer_ = nullptr;
            memcpy(buffer_,buffer+headindex,datasize);
            memcpy(buffer,buffer_,datasize);
            headindex = 0;
            tailindex -= n;
        }
    }
    else
    {
        std::cerr << "write error!" << std::endl;
    }
}

void Buffer::appendInBuffer(const void* message)
{
    int n = strlen(static_cast<const char*>(message));
    if(n < getFreeSize())
    {
        memcpy(buffer+tailindex,const_cast<void*>(message),n);
        tailindex += n;
    }
    else
    {
        while(n > getFreeSize())
        {
            KBufferSize *= 2;
            level  = KBufferSize / 2;
            buffer = static_cast<char*>(realloc(buffer,KBufferSize));
        }
        memcpy(buffer+tailindex,const_cast<void*>(message),n);
        tailindex += n;
    }
}

void* Buffer::getInBuffer()
{   
    int datasize = getDataSize();
    void* chlist = malloc(datasize);
    bzero(chlist,datasize);
    memcpy(chlist,buffer+headindex,datasize);
    return chlist;
}
    
    
int main(int argc,char** argv)
{
    Buffer inputbuffer;
    int fd = open("/home/uuchen/hello.txt",O_RDWR|O_APPEND);
    
    cout << "fd: " << fd << endl;
    inputbuffer.readInSocket(fd);
    cout << "--------readInSocket() and getInBuffer()-----------" << endl;
    cout << static_cast<char*>(inputbuffer.getInBuffer());
    cout << "DataSize: " << inputbuffer.getDataSize() << endl;
    cout << "FreeSize: " << inputbuffer.getFreeSize() << endl;
    cout << "HeadIndex: " << inputbuffer.getHeadIndex() << endl;
    cout << "TailIndex: " << inputbuffer.getTailIndex() << endl;
    
    cout << "--------writeInsocket()----------" << endl;
    inputbuffer.writeInSocket(fd);
    cout << "DataSize: " << inputbuffer.getDataSize() << endl;
    cout << "FreeSize: " << inputbuffer.getFreeSize() << endl;
    cout << "HeadIndex: " << inputbuffer.getHeadIndex() << endl;
    cout << "TailIndex: " << inputbuffer.getTailIndex() << endl;
   
    cout << "---------appendInBuffer()------------" << endl;
    inputbuffer.appendInBuffer("hello world!");
    cout << "DataSize: " << inputbuffer.getDataSize() << endl;
    cout << "FreeSize: " << inputbuffer.getFreeSize() << endl;
    cout << "HeadIndex: " << inputbuffer.getHeadIndex() << endl;
    cout << "TailIndex: " << inputbuffer.getTailIndex() << endl;

    close(fd);
    return 0;
}


































