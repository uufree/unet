/*************************************************************************
	> File Name: BufferClientTest.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月30日 星期三 20时02分45秒
 ************************************************************************/

#include<iostream>
#include"../Socket.h"
#include"../InetAddress.h"
#include"../Alloc.h"
#include"../Buffer.h"

using namespace unet::base;

int main(int argc,char** argv)
{
    InetAddress serverAddr(7777,"127.0.0.1");
    Socket confd(CONNECT);
    confd.connect(serverAddr);
    std::cout << "Client ConnectFD: " << confd.getFd() << std::endl;
    
    Buffer buffer(confd.getFd());
    char buf[16];
    bzero(buf,16);

    while(1)
    {
        int j = buffer.readInSocket();
        std::cout << "Client Read: " << j << std::endl;
        buffer.readInBuffer(buf,13);
        std::cout << buf << std::endl;

        buffer.writeInBuffer("Hello,Server!",13);
        int i = buffer.writeInSocket();
        std::cout << "Client Write: " << i << std::endl;
        
        sleep(1);
        bzero(buf,16);
    }

    return 0;
}

