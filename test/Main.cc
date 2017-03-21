/*************************************************************************
	> File Name: Main.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月19日 星期日 12时44分29秒
 ************************************************************************/

#include"../TcpServ"


int main(int argc,char** argv)
{
    unet::net::EventLoop loop;
    unet::net::InetAddress addr(9999);
    unet::net::TcpServer server(&loop,addr);
    server.start();

    return 0;
}

