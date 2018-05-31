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
//    confd.setNonBlockAndCloseOnExec();
    
    buffer.setBlock();
//    buffer.setNonBlock();

    char buf[4000];
    bzero(buf,4000);
    
    char sendBuf[4000];
    memset(sendBuf,'c',4000);

    while(1)
    {
//        int j = buffer.readInSocket();
        int j = confd.read(buf,4000);
        std::cout << "Client Read: " << j << std::endl;
//        buffer.readInBuffer(buf,4096);
        std::cout << buf << std::endl;
        
        
//        int i = buffer.writeInBuffer(sendBuf,4096);
        int i = confd.write(sendBuf,4000);
        std::cout << "Client Write: " << i << std::endl;
//        buffer.writeInSocket();
        
        sleep(1);
        bzero(buf,4000);
    }

    return 0;
}

