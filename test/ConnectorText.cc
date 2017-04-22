/*************************************************************************
	> File Name: ConnectorText.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月24日 星期五 16时12分27秒
 ************************************************************************/

#include"../Socket.h"
#include"../InetAddress.h"

int main(int argc,char** argv)
{
    int confd = unet::net::socket::socket();
    unet::net::InetAddress serveraddr("120.25.0.40",7777);
    
    unet::net::socket::connect(confd,&serveraddr);

    std::cout << "already connect!" << std::endl;
    
    return 0;
}

