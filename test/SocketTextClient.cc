/*************************************************************************
	> File Name: SocketTextClient.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月17日 星期五 22时11分48秒
 ************************************************************************/

#include"../Socket.h"
#include"../InetAddress.h"
#include<iostream>
#include<sys/poll.h>

int main(int argc,char** argv)
{
    int confd = unet::net::socket::socket();
    unet::net::InetAddress serveraddr("127.0.0.1",7777);

    unet::net::socket::connect(confd,&serveraddr);
    std::cout << "confd: " << confd << std::endl;
/*
    int flag = 0;
    std::string str = "hello server!";
    char buf[1024];
    bzero(buf,1024);

    while(1)
    {
        std::cout << "-------------------------" << std::endl;
        if(flag < 5)
        {
            int m = write(confd,str.c_str(),str.size());
            std::cout << "write size: " << m << std::endl;
        }
        int n = read(confd,buf,sizeof(buf));
        std::cout << "read size: " << n << std::endl; 
        ++flag;
        if(flag < 5)
        {
            std::cout << buf << std::endl;
            bzero(buf,1024);
        }
        else
        {
            unet::net::socket::shutdownWrite(confd);
            std::cout << buf << std::endl;
            bzero(buf,1024);
        }

        if(flag > 10)
        {
//            unet::net::socket::shutdownRead(confd);
            ::close(confd);
            break;
        } 
    }
*/
/* 
    struct pollfd pfd;
    pfd.fd = confd;
    pfd.events = POLLOUT && POLLIN;
    int count = ::poll(&pfd,1,64000);

    if(count == 1)
    {
        if(pfd.revents & POLLIN)
            std::cout << "Client confd can read!" << std::endl;
        else if(pfd.revents & POLLOUT)
            std::cout << "Client confd can write!" << std::endl;
        else
            std::cout << "error!" << std::endl;
    }
    else
        std::cout << "nothing happened!" << std::endl;
*/
    
    char buf[16] = "hello,world!";
    write(confd,buf,16);
    
    sleep(10);
    unet::net::socket::close(confd);
    std::cout << "client confd close" << std::endl;
    return 0;
}












