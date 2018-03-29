/*************************************************************************
	> File Name: Server.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年03月30日 星期五 00时10分14秒
 ************************************************************************/

#include<iostream>
#include"../Socket.h"
#include"../InetAddress.h"

using namespace unet;
using namespace unet::base;

int main(int argc,char** argv)
{
    Socket listenSocket(LISTEN); 
    InetAddress serverAddr(6666);
    listenSocket.bind(serverAddr);
    listenSocket.listen();
    std::cout << "listenfd: " << listenSocket.getFd() << std::endl;

    int connectfd = listenSocket.accept();
    std::cout << "connectfd: " << connectfd << std::endl;
    

    return 0;
}

