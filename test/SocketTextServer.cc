/*************************************************************************
	> File Name: SocketTextServer.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月17日 星期五 19时36分18秒
 ************************************************************************/

#include"../Socket.h"
#include"../InetAddress.h"
#include<iostream>


int main(int argc,char** argv)
{
    int listenfd = unet::net::socket::socket();
    unet::net::InetAddress addr(7777);

    unet::net::socket::bind(listenfd,&addr);
    unet::net::socket::listen(listenfd);
    std::cout << "listenfd: " << listenfd << std::endl;

    int confd = unet::net::socket::accept(listenfd);
    std::cout << "confd: " << confd << std::endl;

    unet::net::socket::close(listenfd);
    unet::net::socket::close(confd);
    return 0;
}

































