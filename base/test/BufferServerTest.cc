/*************************************************************************
	> File Name: BufferServerTest.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月30日 星期三 19时54分35秒
 ************************************************************************/

#include<iostream>
#include"../Socket.h"
#include"../InetAddress.h"
#include"../Alloc.h"
#include"../Buffer.h"

using namespace unet::base;

int main(int argc,char** argv)
{
    InetAddress serverAddr(7777);
    Socket listenfd(LISTEN);
    listenfd.bind(serverAddr);
    listenfd.listen();
    
    std::cout << "Server ListenFD: " << listenfd.getFd() << std::endl;
    int confd = listenfd.accept();
    std::cout << "Server ConnectFD: " << confd << std::endl;
    
    Socket confds(confd);
//    confds.setNonBlockAndCloseOnExec();
    Buffer buffer(confd);
    
    buffer.setBlock();
//    buffer.setNonBlock();

    char buf[4000];
    memset(buf,'\0',4000);
    
    char sendBuf[4000];
    memset(sendBuf,'u',4000);

    while(1)
    {
        buffer.writeInBuffer(sendBuf,4000);
        std::cout << "========================" << std::endl;
        std::cout << "Read Free Size: " << buffer.readFreeSize() << std::endl;
        std::cout << "Write Free Size: " << buffer.writeFreeSize() << std::endl;
        std::cout << "========================" << std::endl;
        int i = buffer.writeInSocket();
        std::cout << "========================" << std::endl;
        std::cout << "Read Free Size: " << buffer.readFreeSize() << std::endl;
        std::cout << "Write Free Size: " << buffer.writeFreeSize() << std::endl;
        std::cout << "========================" << std::endl;
        std::cout << "Server Write: " << i << std::endl;
    
        int j = buffer.readInSocket();
        std::cout << "========================" << std::endl;
        std::cout << "Read Free Size: " << buffer.readFreeSize() << std::endl;
        std::cout << "Write Free Size: " << buffer.writeFreeSize() << std::endl;
        std::cout << "========================" << std::endl;
        std::cout << "Server Read: " << j << std::endl;
        buffer.readInBuffer(buf,4000);
        std::cout << "========================" << std::endl;
        std::cout << "Read Free Size: " << buffer.readFreeSize() << std::endl;
        std::cout << "Write Free Size: " << buffer.writeFreeSize() << std::endl;
        std::cout << "========================" << std::endl;

        std::cout << buf << std::endl;
        

        sleep(1);
        bzero(buf,4000);
    }
    
    return 0;
}

