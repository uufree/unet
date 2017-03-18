/*************************************************************************
	> File Name: SocketTextServer.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月17日 星期五 19时36分18秒
 ************************************************************************/

#include"SocketText.h"


int main(int argc,char** argv)
{
    int listenfd = net::socket();
    net::InetAddress addr(7777);

    net::bind(listenfd,&addr);
    net::listen(listenfd);
    cout << "listenfd: " << listenfd << endl;

    int confd = net::accept(listenfd);
    cout << "confd: " << confd << endl;

    net::close(listenfd);
    net::close(confd);
    return 0;
}

































