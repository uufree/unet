/*************************************************************************
	> File Name: SignalEventTest.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年06月01日 星期五 20时22分14秒
 ************************************************************************/

#include<iostream>
#include"../SignalEvent.h"
#include"../base/Thread.h"
#include<unistd.h>

using namespace unet;
using namespace unet::base;

void threadFunc()
{
    while(1)
    {
        sendSignal(U_USRSTOP);
        sendSignal(U_USRRESTART);
        sendSignal(U_USRINT);
        sleep(1);
    }
}

int main(int argc,char** argv)
{
    SignalEvent event;
    Thread threadA(std::bind(&threadFunc));
    threadA.start();
    event.handleSignal();
    return 0;
}
