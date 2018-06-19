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

/*阻塞版本正常*/
/*非阻塞版本未发现问题*/

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

    char* buf = new char[4096];
    memset(buf,'u',4096);

    while(1)
    {
        int j = buffer.readInSocket();
        std::cout << "Client Read: " << j << std::endl;
        buffer.readInBuffer(buf,4096);
//        std::cout << buf << std::endl;
        std::cout << "buflen: " << strlen(buf) << std::endl;
        
        memset(buf,'c',4096);
        int i = buffer.writeInBuffer(buf,4096);
        std::cout << "Client Write: " << i << std::endl;
        buffer.writeInSocket();
        
        sleep(1);
        bzero(buf,4096);
    }

    return 0;
}

