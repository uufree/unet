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
#include<sys/poll.h>

int main(int argc,char** argv)
{
    int listenfd = unet::net::socket::socket();
    unet::net::InetAddress addr(7777);

    unet::net::socket::bind(listenfd,&addr);
    unet::net::socket::listen(listenfd);
    std::cout << "listenfd: " << listenfd << std::endl;

    int confd = unet::net::socket::accept(listenfd);
    std::cout << "confd: " << confd << std::endl;
/*    
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
*/
    
    struct pollfd pfd;
    pfd.fd = confd;
    pfd.events = POLLOUT && POLLIN;
    int count = ::poll(&pfd,1,64000);

    if(count == 1)
    {
        if(pfd.revents & POLLIN)
            std::cout << "Server confd can read!" << std::endl;
        else if(pfd.revents & POLLOUT)
            std::cout << "Server confd can write!" << std::endl;
        else
            std::cout << "error!" << std::endl;
    }
    else
        std::cout << "nothing happened!" << std::endl;
   
    sleep(10);
    unet::net::socket::close(listenfd);
    std::cout << "listen socket close" << std::endl;
    unet::net::socket::close(confd);
    std::cout << "server confd close" << std::endl;
    return 0;
}

































