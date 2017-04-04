/*************************************************************************
	> File Name: SocketTextClient.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月17日 星期五 22时11分48秒
 ************************************************************************/

#include"../Socket.h"
#include"../InetAddress.h"
#include"../File.h"
#include<iostream>
#include<sys/poll.h>
#include<vector>
#include<unistd.h>
#include"../Buffer.h"

using namespace unet;
using namespace unet::net;

int main(int argc,char** argv)
{
//    std::vector<int> vec;
    int confd = unet::net::socket::socket();
    InetAddress serveraddr("127.0.0.1",7777);
    unet::net::socket::connect(confd,&serveraddr);
    std::cout << "confd: " << confd << std::endl;

    Buffer outputbuffer(confd);
    File chenuu("/home/uuchen/chenuu.jpeg") ;
    char* buf;

    while(1)
    {
        outputbuffer.readInSocket();
        buf = outputbuffer.getCompleteMessageInBuffer();
        chenuu.writen(buf,strlen(buf));
        if(chenuu.getWriteSize() < 0)
            break;
        
        ::sleep(1);
    }

    return 0;
}












