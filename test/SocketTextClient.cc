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
#include<vector>

int main(int argc,char** argv)
{
    std::vector<int> vec;
    unet::net::InetAddress serveraddr("127.0.0.1",7777);
    for(int i=0;i<5;++i)
    {
        int confd = unet::net::socket::socket();
        vec.push_back(confd);
        unet::net::socket::connect(confd,&serveraddr);
        std::cout << "confd: " << confd << std::endl;
        sleep(2);
    }

/*    
    char buf[16] = "hello,world!";
    write(confd,buf,16);
*/
/*
    char buf[16] = "hello, server!";
    while(1)
    {  
        sleep(1);
        ::write(confd,buf,16);
        std::cout << "sending~" << std::endl;
    }
*/ 

    sleep(10);
    
    for(auto iter=vec.begin();iter!=vec.end();++iter)
        unet::net::socket::close(*iter);

    std::cout << "client confd close" << std::endl;
    return 0;
}












