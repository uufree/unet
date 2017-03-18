/*************************************************************************
	> File Name: SocketTextClient.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月17日 星期五 22时11分48秒
 ************************************************************************/

#include"SocketText.h"

int main(int argc,char** argv)
{
    int confd = net::socket();
    net::InetAddress serveraddr("127.0.0.1",7777);

    net::connect(confd,&serveraddr);
    cout << "confd: " << confd << endl;

    net::close(confd);
    return 0;
}












