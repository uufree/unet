/*************************************************************************
	> File Name: AsyncTcpServerTest.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年04月18日 星期二 17时19分56秒
 ************************************************************************/

#include"../AsyncTcpServer.h"

int main(int argc,char** argv)
{
    unet::net::socket::InetAddress serveraddr(7777);
    unet::net::AsyncTcpServer server(serveraddr);
    
    return 0;
}
    

    



