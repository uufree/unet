/*************************************************************************
	> File Name: EventLoopThreadTest.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月28日 星期二 23时48分15秒
 ************************************************************************/

#include"../EventLoopThread.h"

using namespace unet;
using namespace unet::thread;

int main(int argc,char** argv)
{
    EventLoopThread thread;
    thread.setThreadCallBack();
    EventLoopThread thread1;
    thread1 = thread;

    thread1.start();

    sleep(10);
    return 0;
}

