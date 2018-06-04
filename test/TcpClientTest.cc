/*************************************************************************
	> File Name: TcpClientTest.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年06月04日 星期一 16时43分14秒
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
    memset(buf,'c',128);
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
    InetAddress serverAddr(7777,"127.0.0.1");
    Socket confd(CONNECT);    
    confd.connect(serverAddr);
    std::cout << "Client ConnectFD: " << confd.getFd() << std::endl;
    
    char* buf = new char[128];
    while(1)
    {
        memset(buf,'c',128);
        int ssize = confd.write(buf,128); 
        std::cout << "client send: " << ssize << std::endl;
        memset(buf,'\0',128);


        int rsize = confd.read(buf,128);
        std::cout << "client recv: " << rsize << std::endl;
        std::cout << "buf: " << buf << std::endl;
        
        sleep(1);
    }
    
    return 0;
}


