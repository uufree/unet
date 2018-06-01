/*************************************************************************
	> File Name: TcpConnectionTest.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年06月01日 星期五 14时36分18秒
 ************************************************************************/

#include<iostream>
#include"../TcpConnection.h"

using namespace unet;
using namespace unet::base;

void handleReadCallBack(TcpConnection::BufferPtr& buffer)
{
    if(buffer->readUsedSize() < 128)
        return;
    std::cout << "handleReadCallBack!" << std::endl;
    char* buf = new char[128];
    memset(buf,'\0',128);
    buffer->readInBuffer(buf,128);
    std::cout << buf << std::endl;
    delete [] buf;
}

void handleWriteCallBack(TcpConnection::BufferPtr& buffer)
{
    if(buffer->writeFreeSize() < 128)
        return;
    std::cout << "handleWriteCallBack!" << std::endl;
    char* buf = new char[128];
    memset(buf,'s',128);
    buffer->writeInBuffer(buf,128);
    delete [] buf;
}

void handleCloseCallBack(int fd)
{
    std::cout << "handleCloseCallBack!" << std::endl;
    std::cout << "close fd: " << fd << std::endl;
}

int main(int argc,char** argv)
{
    InetAddress serverAddr(7777);
    Socket listenfd(LISTEN);
    listenfd.bind(serverAddr);
    listenfd.listen();

    std::cout << "Server ListenFD: " << listenfd.getFd() << std::endl;
    int confd = listenfd.accept();
    std::cout << "Server ConnectFD: " << confd << std::endl;
    
    TcpConnection con(confd);
    con.setNonBlockAndCloseOnExec();
    con.setReadCallBack(std::bind(&handleReadCallBack,std::placeholders::_1));
    con.setWriteCallBack(std::bind(&handleWriteCallBack,std::placeholders::_1));
    con.setCloseCallBack(std::bind(&handleCloseCallBack,std::placeholders::_1));

    while(1)
    {
        con.handleWrite();
        std::cout << "========================" << std::endl;
        std::cout << "Read Free Size: " << con.readFreeSize() << std::endl;
        std::cout << "Write Free Size: " << con.writeFreeSize() << std::endl;
        std::cout << "========================" << std::endl;
        
        con.handleRead();
        std::cout << "========================" << std::endl;
        std::cout << "Read Free Size: " << con.readFreeSize() << std::endl;
        std::cout << "Write Free Size: " << con.writeFreeSize() << std::endl;
        std::cout << "========================" << std::endl;
        
        sleep(1);
    }
    
    return 0;
}

