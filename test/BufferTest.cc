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
#include"../Buffer.h"

using namespace std;
    
int main(int argc,char** argv)
{
    unet::net::Buffer inputbuffer;
    int fd = open("/home/uuchen/hello.txt",O_RDWR|O_APPEND);
    cout << "fd: " << fd << endl;
    inputbuffer.readInSocket(fd);
    cout << "--------readInSocket() and getInBuffer()-----------" << endl;
    cout << static_cast<char*>(inputbuffer.getInBuffer());
    cout << "DataSize: " << inputbuffer.getDataSize() << endl;
    cout << "FreeSize: " << inputbuffer.getFreeSize() << endl;
    cout << "HeadIndex: " << inputbuffer.getHeadIndex() << endl;
    cout << "TailIndex: " << inputbuffer.getTailIndex() << endl;
    cout << "BufferSize: " << inputbuffer.getBufferSize() << endl;
    
    cout << "--------writeInsocket()----------" << endl;
    inputbuffer.writeInSocket(fd);
    cout << "DataSize: " << inputbuffer.getDataSize() << endl;
    cout << "FreeSize: " << inputbuffer.getFreeSize() << endl;
    cout << "HeadIndex: " << inputbuffer.getHeadIndex() << endl;
    cout << "TailIndex: " << inputbuffer.getTailIndex() << endl;
    cout << "BufferSize: " << inputbuffer.getBufferSize() << endl;
/*   
    cout << "---------appendInBuffer()------------" << endl;
    inputbuffer.appendInBuffer("hello world!");
    cout << "DataSize: " << inputbuffer.getDataSize() << endl;
    cout << "FreeSize: " << inputbuffer.getFreeSize() << endl;
    cout << "HeadIndex: " << inputbuffer.getHeadIndex() << endl;
    cout << "TailIndex: " << inputbuffer.getTailIndex() << endl;
*/
    close(fd);
    return 0;
}


































