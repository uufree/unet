/*************************************************************************
	> File Name: BufferTest.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月15日 星期三 15时31分52秒
 ************************************************************************/

#include<iostream>
#include<string.h>
#include<unistd.h>
#include"../Buffer.h"
#include"../File.h"

using namespace std;
using namespace unet::net;
using namespace unet;

int main(int argc,char** argv)
{
    char buf[1024] = "hello,world!";
    File uuchen("/home/uuchen/uuchen.jpeg");
    Buffer inputbuffer(5);

    for(int i=0;i<10;++i)
    {
        cout << "-----------------------------" << endl;
        inputbuffer.appendInBuffer(buf);
        cout << inputbuffer.getCompleteMessageInBuffer() << endl;
        sleep(0.5);
    }
        

    /*
    inputbuffer.appendInBuffer(buf);
    char* buff = inputbuffer.getCompleteMessageInBuffer();
    std::cout << buff << std::endl;
*/

/*    
    inputbuffer.appendInBuffer("hello world!");
    cout << "--------readInSocket() and getCompleteMessageInBuffer()-----------" << endl;
    cout << inputbuffer.getCompleteMessageInBuffer() << endl;
    cout << "DataSize: " << inputbuffer.getDataSize() << endl;
    cout << "FreeSize: " << inputbuffer.getFreeSize() << endl;
    cout << "HeadIndex: " << inputbuffer.getHeadIndex() << endl;
    cout << "TailIndex: " << inputbuffer.getTailIndex() << endl;
    cout << "BufferSize: " << inputbuffer.getBufferSize() << endl;
*/
/*    
    cout << "--------writeInsocket()----------" << endl;
    cout << "DataSize: " << inputbuffer.getDataSize() << endl;
    cout << "FreeSize: " << inputbuffer.getFreeSize() << endl;
    cout << "HeadIndex: " << inputbuffer.getHeadIndex() << endl;
    cout << "TailIndex: " << inputbuffer.getTailIndex() << endl;
    cout << "BufferSize: " << inputbuffer.getBufferSize() << endl;
*//*   
    cout << "---------appendInBuffer()------------" << endl;
    inputbuffer.appendInBuffer("hello world!");
    cout << "DataSize: " << inputbuffer.getDataSize() << endl;
    cout << "FreeSize: " << inputbuffer.getFreeSize() << endl;
    cout << "HeadIndex: " << inputbuffer.getHeadIndex() << endl;
    cout << "TailIndex: " << inputbuffer.getTailIndex() << endl;
*/
    return 0;
}


































