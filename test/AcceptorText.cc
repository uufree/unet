/*************************************************************************
	> File Name: Acceptor.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月19日 星期日 01时45分15秒
 ************************************************************************/

#include"../Acceptor.h"

int main(int argc,char** argv)
{
    unet::net::InetAddress addr(9999);
    unet::net::Acceptor acceptor(&addr);
    return 0;
}

