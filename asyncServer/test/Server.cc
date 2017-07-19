/*************************************************************************
	> File Name: AsyncTcpServerTest.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年04月18日 星期二 17时19分56秒
 ************************************************************************/

#include"../AsyncTcpServer.h"

void readCallBack(unet::net::Buffer* inputBuffer,unet::net::Buffer* outputBuffer)
{
    std::string message;
    inputBuffer->getCompleteMessageInBuffer(message);

    std::cout << message << std::endl;
}

int main(int argc,char** argv)
{
    unet::net::socket::InetAddress serveraddr(7777);
    unet::net::AsyncTcpServer server(serveraddr);
    
    server.setReadCallBack(std::bind(&readCallBack,std::placeholders::_1,std::placeholders::_2));
    server.start();

    return 0;
}

/*
using namespace unet;

int main(int argc,char** argv)
{
    net::socket::InetAddress serverAddr(7777);
    net::socket::Socket listenfd(net::socket::LISTEN);
    net::socket::bind(listenfd,serverAddr);
    net::socket::listen(listenfd);

    int confd = net::socket::accept(listenfd);
    
    net::Buffer serverBuffer(confd);

    std::string message;
    serverBuffer.readInSocket();
    serverBuffer.getCompleteMessageInBuffer(message);
    std::cout << message << std::endl;

    return 0;
}
*/
