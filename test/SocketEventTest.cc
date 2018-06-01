/*************************************************************************
	> File Name: SocketEventTest.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年06月01日 星期五 15时44分04秒
 ************************************************************************/

#include<iostream>
#include"../SocketEvent.h"
#include"../type.h"
#include<memory>

using namespace unet;

int main(int argc,char** argv)
{
    std::shared_ptr<SocketEvent> event(new ListenSocketEvent(10,U_LISTEN_SOCKET)); 
    event->handleEvent(U_READ);
    return 0;
}
