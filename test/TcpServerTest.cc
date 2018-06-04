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

int main(int argc,char** argv)
{
    InetAddress serverAddr(7777);
    TcpServer server(serverAddr);
    server.start();

    return 0;
}
