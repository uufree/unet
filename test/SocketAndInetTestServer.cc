/*************************************************************************
	> File Name: SocketAndInetTestServer.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年07月11日 星期二 11时15分48秒
 ************************************************************************/

#include<iostream>
#include"../Socket.h"
#include"../InetAddress.h"

using namespace unet::base;

int main(int argc,char** argv)
{
    InetAddress serverAddr(7777);
    Socket listenfds(LISTEN); 
    Socket listenfd(std::move(listenfds));

    std::cout << "=============================" << std::endl;
    std::cout << "Listen Socket Messgae: " << std::endl;
    std::cout << "Is Used: " << listenfd.isUsed() << std::endl;
    std::cout << "Is ReuseAddr: " << listenfd.isReuseAddr() << std::endl;
    std::cout << "Is Linger: " << listenfd.isLinger() << std::endl;
    std::cout << "Is KeepAlive: " << listenfd.isKeepAlive() << std::endl;
    std::cout << "Is NoDelay: " << listenfd.isNoDelay() << std::endl;
    std::cout << "Is NoBlock: " << listenfd.isNonBlockAndCloseOnExec() << std::endl;
    std::cout << "Recv Buf: " << listenfd.getRecvBuf() << std::endl;
    std::cout << "Send Buf: " << listenfd.getSendBuf() << std::endl;
    std::cout << "Recv LowAt: " << listenfd.getRecvLowAt() << std::endl;
    std::cout << "Send LowAt: " << listenfd.getSendLowAt() << std::endl;
    std::cout << "=============================" << std::endl;
    
    listenfd.setRecvBuf(10);
    listenfd.setSendBuf(10);
    listenfd.setCloseRST();
    
    std::cout << "=============================" << std::endl;
    std::cout << "Listen Socket Messgae: " << std::endl;
    std::cout << "Is Used: " << listenfd.isUsed() << std::endl;
    std::cout << "Is ReuseAddr: " << listenfd.isReuseAddr() << std::endl;
    std::cout << "Is Linger: " << listenfd.isLinger() << std::endl;
    std::cout << "Is KeepAlive: " << listenfd.isKeepAlive() << std::endl;
    std::cout << "Is NoDelay: " << listenfd.isNoDelay() << std::endl;
    std::cout << "Is NoBlock: " << listenfd.isNonBlockAndCloseOnExec() << std::endl;
    std::cout << "Recv Buf: " << listenfd.getRecvBuf() << std::endl;
    std::cout << "Send Buf: " << listenfd.getSendBuf() << std::endl;
    std::cout << "Recv LowAt: " << listenfd.getRecvLowAt() << std::endl;
    std::cout << "Send LowAt: " << listenfd.getSendLowAt() << std::endl;
    std::cout << "=============================" << std::endl;

    listenfd.bind(serverAddr);
    listenfd.listen();
    
    std::cout << "Server ListenFD: " << listenfd.getFd() << std::endl;
    int confd = listenfd.accept();
    std::cout << "Server ConnectFD: " << confd << std::endl;

    Socket confds(confd);
    confds.setRecvBuf(10);
    confds.setSendBuf(10);
    std::cout << "=============================" << std::endl;
    std::cout << "Connect Socket Messgae: " << std::endl;
    std::cout << "Is Used: " << confds.isUsed() << std::endl;
    std::cout << "Is ReuseAddr: " << confds.isReuseAddr() << std::endl;
    std::cout << "Is Linger: " << confds.isLinger() << std::endl;
    std::cout << "Is KeepAlive: " << confds.isKeepAlive() << std::endl;
    std::cout << "Is NoDelay: " << confds.isNoDelay() << std::endl;
    std::cout << "Is NoBlock: " << confds.isNonBlockAndCloseOnExec() << std::endl;
    std::cout << "Recv Buf: " << confds.getRecvBuf() << std::endl;
    std::cout << "Send Buf: " << confds.getSendBuf() << std::endl;
    std::cout << "Recv LowAt: " << confds.getRecvLowAt() << std::endl;
    std::cout << "Send LowAt: " << confds.getSendLowAt() << std::endl;
    std::cout << "=============================" << std::endl;
    std::string messageClient("hello,client!");
    std::string messageServer;
   
    char buffer[32];
    bzero(buffer,32);

    while(1)
    {
        int i = confds.write(messageClient.c_str(),messageClient.size());
        std::cout << "Server Write: " << i << std::endl;
        int j = confds.read(buffer,13); 
        std::cout << "Server Read: " << j << std::endl;
        std::cout << buffer << std::endl;
        
        sleep(1);
        bzero(buffer,32);
    }

    return 0;
}
