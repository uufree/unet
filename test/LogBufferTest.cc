/*************************************************************************
	> File Name: LogBufferTest.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年06月03日 星期日 20时49分44秒
 ************************************************************************/

#include<iostream>
#include"../LogBuffer.h"

using namespace unet;

int main(int argc,char** argv)
{
    ::putenv((char*)"NAME=a");
    LogBufferQueue buffer;
    LogBufferQueue::startWriteThread();
    for(int i=0;i<1025;i++)
        buffer.append("Hello,I'm Main Thread!\n");
    std::cout << "append finish!" << std::endl;
    while(1);

    return 0;
}
