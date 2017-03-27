/*************************************************************************
	> File Name: TcpServerText.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月21日 星期二 19时57分00秒
 ************************************************************************/

#include"../TcpServer.h"
#include<string>

using namespace unet;
using namespace unet::net;

void writeForClient(Buffer* inputbuffer,Buffer* outputbuffer)
{
    std::string str = "hello,client!";
    outputbuffer->appendInBuffer(str.c_str());
}

int main(int argc,char** argv)
{
    unet::net::InetAddress serveraddr(7777);
    unet::net::TcpServer server(&serveraddr);
    server.setWriteCallBack(std::bind(&writeForClient,std::placeholders::_1,std::placeholders::_2));

    server.start();

    return 0;
}


