/*************************************************************************
	> File Name: InetAddressTest.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月17日 星期五 17时10分57秒
 ************************************************************************/
 
#include<iostream>
#include"../InetAddress.h"

using namespace std;

int main(int argc,char** argv)
{
    unet::net::InetAddress ipv4addr(9999);
    
    cout << "--------------------------------" << endl;
    cout << ipv4addr.getIpString() << endl;

    cout << "--------------------------------" << endl;
    cout << ipv4addr.getIp() << endl;
    
    cout << "--------------------------------" << endl;
    cout << ipv4addr.getPort() << endl;
    

    return 0;
}
   



















