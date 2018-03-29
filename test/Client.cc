/*************************************************************************
	> File Name: Client.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年03月30日 星期五 00时10分23秒
 ************************************************************************/

#include<iostream>
#include"../InetAddress.h"
#include"../Socket.h"

using namespace unet;
using namespace unet::base;

int main(int argc,char** argv)
{
    Socket connectSocket(CONNECT);
    InetAddress serverAddr(6666);
    connectSocket.connect(serverAddr);
    std::cout << "listenfd: " << connectSocket.getFd() << std::endl;
    
    std::string message("hello,world!");
    while(1)
    {
        connectSocket.blockWrite(message);
        sleep(3);
    }

    return 0;
}


