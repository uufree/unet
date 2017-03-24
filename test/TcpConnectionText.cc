/*************************************************************************
	> File Name: TcpConnectionText.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月19日 星期日 14时42分40秒
 ************************************************************************/

#include"../TcpConnection.h"

typedef std::shared_ptr<unet::net::TcpConnection> TcpConnectionPtr;

//智能指针智能在第一次声明时就明确指向的目标

int main(int argc,char** argv)
{
    unet::net::TcpConnection tcp(3);
    unet::net::TcpConnection* ptr = new unet::net::TcpConnection(3);
    TcpConnectionPtr uptr(nullptr);

    return 0;
}
