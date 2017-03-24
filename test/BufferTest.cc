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

using namespace std;
    
int main(int argc,char** argv)
{
    unet::net::Buffer inputbuffer;
    inputbuffer.appendInBuffer("hello world!");
    cout << "--------readInSocket() and getCompleteMessageInBuffer()-----------" << endl;
    cout << inputbuffer.getCompleteMessageInBuffer() << endl;
    cout << "DataSize: " << inputbuffer.getDataSize() << endl;
    cout << "FreeSize: " << inputbuffer.getFreeSize() << endl;
    cout << "HeadIndex: " << inputbuffer.getHeadIndex() << endl;
    cout << "TailIndex: " << inputbuffer.getTailIndex() << endl;
    cout << "BufferSize: " << inputbuffer.getBufferSize() << endl;
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


































