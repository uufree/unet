/*************************************************************************
	> File Name: SocketTextServer.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月17日 星期五 19时36分18秒
 ************************************************************************/

#include"../Socket.h"
#include"../InetAddress.h"
#include<iostream>
#include<string>

int main(int argc,char** argv)
{
    int listenfd = unet::net::socket::socket();
    unet::net::InetAddress addr(7777);

    unet::net::socket::bind(listenfd,&addr);
    unet::net::socket::listen(listenfd);
    std::cout << "listenfd: " << listenfd << std::endl;

    int confd = unet::net::socket::accept(listenfd);
    std::cout << "confd: " << confd << std::endl;
    
    std::string str = "hello client!";
    char buf[1024];
    bzero(buf,1024);
    while(1)
    {
        std::cout << "-----------------------------" << std::endl;
        int n = read(confd,buf,sizeof(buf));
        std::cout << "read size: " << n << std::endl;
        int m = write(confd,str.c_str(),str.size());
        std::cout << "write size: " << m << std::endl;
        std::cout << buf << std::endl;
        sleep(1);
    }

    unet::net::socket::close(listenfd);
    std::cout << "socket close" << std::endl;
    unet::net::socket::close(confd);
    return 0;
}

































