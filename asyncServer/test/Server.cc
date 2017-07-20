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

    std::cout << "readCallBack: " << message << std::endl;
}

int main(int argc,char** argv)
{
    unet::net::socket::InetAddress serveraddr(7777);
    unet::net::AsyncTcpServer server(serveraddr,2);
    server.setReadCallBack(std::bind(&readCallBack,std::placeholders::_1,std::placeholders::_2));
    
    server.start();

    return 0;
}

