/*************************************************************************
	> File Name: SocketAndInetTestClient.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年07月11日 星期二 11时15分28秒
 ************************************************************************/

#include<iostream>
#include"../Socket.h"
#include"../InetAddress.h"

using namespace unet::base;

int main(int argc,char** argv)
{    
    InetAddress serverAddr(7777,"127.0.0.1");
    Socket confds(CONNECT);
    Socket confd = std::move(confds);
    confd.connect(serverAddr);
    std::cout << "Client ConnectFD: " << confd.getFd() << std::endl;
    std::string messageServer("hello,server!");
    std::string messageClient;
    
    char buffer[32];
    bzero(buffer,32);

    while(1)
    {
        int i = confd.read(buffer,32);
        std::cout << "Client Read: " << i << std::endl;
        std::cout << buffer << std::endl;
        int j = confd.write(messageServer.c_str(),messageServer.size());
        std::cout << "Client Write: " << j << std::endl;

        sleep(1);
        bzero(buffer,32);
    }

    return 0;
}


