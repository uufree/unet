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

/*阻塞版本正常*/
/*非阻塞版本未发现问题*/

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

    char* buf = new char[4096];
    memset(buf,'s',4096);

    while(1)
    {
        buffer.writeInBuffer(buf,4096);
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
    
        memset(buf,'0',4096);
        int j = buffer.readInSocket();
        std::cout << "========================" << std::endl;
        std::cout << "Read Free Size: " << buffer.readFreeSize() << std::endl;
        std::cout << "Write Free Size: " << buffer.writeFreeSize() << std::endl;
        std::cout << "========================" << std::endl;
        std::cout << "Server Read: " << j << std::endl;
        buffer.readInBuffer(buf,4096);
        std::cout << "========================" << std::endl;
        std::cout << "Read Free Size: " << buffer.readFreeSize() << std::endl;
        std::cout << "Write Free Size: " << buffer.writeFreeSize() << std::endl;
        std::cout << "========================" << std::endl;
        
        std::cout << buf << std::endl;
        std::cout << "buflen: " << strlen(buf) << std::endl; 

        sleep(1);
        memset(buf,'s',4096);
    }
    
    return 0;
}

