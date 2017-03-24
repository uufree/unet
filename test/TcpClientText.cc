/*************************************************************************
	> File Name: TcpClientText.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月24日 星期五 16时25分26秒
 ************************************************************************/

#include"../TcpClient.h"

int main(int argc,char** argv)
{
    unet::net::InetAddress addr(9999);
    unet::net::TcpClient client(&addr);
    return 0;
}


