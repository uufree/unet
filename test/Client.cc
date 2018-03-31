/*************************************************************************
	> File Name: Client.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年03月30日 星期五 00时10分23秒
 ************************************************************************/

#include<iostream>
#include"../InetAddress.h"
#include"../Socket.h"
#include"../Buffer.h"

using namespace unet;
using namespace unet::base;

int main(int argc,char** argv)
{
    Socket connectSocket(CONNECT);
    InetAddress serverAddr(6666);
    connectSocket.connect(serverAddr);
    connectSocket.setNonBlockAndCloseOnExec();
    std::cout << "listenfd: " << connectSocket.getFd() << std::endl;
    
    std::string message("hello,world!");
    Buffer* buffer = new StringBuffer(connectSocket.getFd());
    for(int i=0;i<3;i++)
    {
        buffer->appendInBuffer(message);
        std::cout << "Buffer Size: " << buffer->size() << std::endl;
        buffer->writeInSocket();
        std::cout << "Buffer Size: " << buffer->size() << std::endl;
        sleep(1);
    }

    return 0;
}


