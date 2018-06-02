/*************************************************************************
	> File Name: EventTest.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年06月02日 星期六 18时00分47秒
 ************************************************************************/

#include<iostream>
#include"../Event.h"
#include"../type.h"

using namespace unet;

int main(int argc,char** argv)
{
    Event timerEvent(U_TIMER);
    Event signalEvent(U_SIGNAL);
    Event listenEvent(U_LISTEN_SOCKET,3);
    Event connectEvent(U_CONNECT_SOCKET,4);
    
    return 0;
}
