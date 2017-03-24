/*************************************************************************
	> File Name: ConnectorText.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月24日 星期五 16时12分27秒
 ************************************************************************/

#include"../Connector.h"

int main(int argc,char** argv)
{
    unet::net::InetAddress serveraddr(9999);
    unet::net::Connector connector(&serveraddr);
    return 0;
}

