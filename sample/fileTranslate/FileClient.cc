/*************************************************************************
	> File Name: FileClient.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年04月03日 星期一 15时46分19秒
 ************************************************************************/

#include"../../Socket.h"
#include"../../InetAddress.h"
#include"../../File.h"

using namespace unet::net::socket;

int main(int argc,char** argv)
{
    int confd = socket();
    unet::net::InetAddress serveraddr("127.0.0.1",7777);
    unet::net::socket::connect(confd,&serveraddr);
    std::cout << "confd: " << confd << std::endl;
    unet::File chenuu("/home/uuchen/chenuu.jpeg");

    char buf[1024];
    while(1)
    {
        int n = ::read(confd,buf,1024);
        chenuu.writen(buf,n);
        std::cout << "read n: " << n << std::endl;
        ::sleep(0.5);
        perror("readInSocket!\n");
    }

    return 0;
}




