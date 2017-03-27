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
    char buf[16] = "hello,world!";
    write(confd,buf,16);
*/
    char buf[16] = "hello, server!";
    while(1)
    {  
        sleep(1);
        ::write(confd,buf,16);
        std::cout << "sending~" << std::endl;
    }
    
    sleep(3);
    unet::net::socket::close(confd);
    std::cout << "client confd close" << std::endl;
    return 0;
}












