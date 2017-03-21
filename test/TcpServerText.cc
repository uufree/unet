/*************************************************************************
	> File Name: TcpServerText.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月21日 星期二 19时57分00秒
 ************************************************************************/

#include"../TcpServer.h"

int main(int argc,char** argv)
{
    unet::net::InetAddress serveraddr(9999);
    unet::net::TcpServer server(&serveraddr);
    return 0;
}


