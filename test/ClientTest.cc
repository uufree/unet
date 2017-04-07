/*************************************************************************
	> File Name: SocketTextClient.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月17日 星期五 22时11分48秒
 ************************************************************************/

#include"../client/TcpClient.h"

using namespace unet;
using namespace unet::net;
using namespace unet::thread;

int main(int argc,char** argv)
{
    int confd = unet::net::socket::socket();
    InetAddress serveraddr("127.0.0.1",7777);
    unet::net::socket::connect(confd,&serveraddr);
    std::cout << "confd: " << confd << std::endl;
    
    char buf[16] = "hello,server!";
    ::write(confd,buf,16);
    
    sleep(4);
    ::close(confd);
    std::cout << "Client Confd alredy close!" << std::endl;
    sleep(2);
    
    
    return 0;
}












