/*************************************************************************
	> File Name: TcpServerTest.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年06月04日 星期一 10时05分41秒
 ************************************************************************/

#include<iostream>
#include"../TcpServer.h"

using namespace unet;
using namespace unet::base;

void handleReadCallBack(TcpConnection::BufferPtr buffer)
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
/*    
    if(buffer->writeFreeSize() < 128)
        return;
    std::cout << "handleWriteCallBack!" << std::endl;
    char* buf = new char[128];
    memset(buf,'c',128);
    buffer->writeInBuffer(buf,128);
    delete [] buf;
*/
}

int main(int argc,char** argv)
{
    InetAddress serverAddr(7777);
    TcpServer server(serverAddr);
//    server.setReadCallBack(std::bind(&handleReadCallBack,std::placeholders::_1));
//    server.setWriteCallBack(std::bind(&handleReadCallBack,std::placeholders::_1));
    server.start();
    
    while(1);
    return 0;
}
