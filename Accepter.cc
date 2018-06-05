/*************************************************************************
	> File Name: Accepter.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年06月02日 星期六 20时15分15秒
 ************************************************************************/

#include"Accepter.h"
#include"Event.h"
#include"Type.h"

namespace unet
{
    Accepter::Accepter(base::InetAddress& serverAddr) :
        u_listen(false),
        u_serverAddr(serverAddr),
        u_listenfd(base::Socket(base::LISTEN))
    {
        u_listenfd.bind(serverAddr);
        u_listenfd.listen();
    }
    
    Accepter::Accepter(Accepter&& accepter) :
        u_listen(accepter.u_listen),
        u_serverAddr(std::move(accepter.u_serverAddr)),
        u_listenfd(std::move(accepter.u_listenfd)),
        u_saveConnectCallBack(accepter.u_saveConnectCallBack),
        u_saveListenCallBack(std::move(accepter.u_saveListenCallBack)),
        u_eraseListenCallBack(std::move(accepter.u_eraseListenCallBack))
    {};

    Accepter& Accepter::operator=(Accepter&& accepter)
    {
        if(accepter == *this)
            return *this;
        u_listen = accepter.u_listen;
        u_serverAddr = std::move(accepter.u_serverAddr);
        u_listenfd = std::move(accepter.u_listenfd);

        return *this;
    }

    Accepter::~Accepter()
    {
        if(u_listen)
            stopListen();
        u_listenfd.close();
    }
    
    int Accepter::startListen()
    {
        /*事件处理回调没有注册的直接退出*/
        if(u_listen)
            return 0;
        if(!u_saveListenCallBack || !u_eraseListenCallBack || 
                !u_saveConnectCallBack)
            return -1;
        
        /*将自身注册进事件处理框架*/
        if(!u_listen)
            u_saveListenCallBack(u_listenfd.getFd());
        u_listen = true;
        return 0;
    
    }
    
    int Accepter::stopListen()
    {
        if(!u_listen)
            return 0;
        if(!u_saveListenCallBack || !u_eraseListenCallBack || 
                !u_saveConnectCallBack)
            return -1;
        
        /*取消注册*/
        u_eraseListenCallBack(u_listenfd.getFd());
        u_listen = false;
        return 0;
    }

    void Accepter::handleRead()
    {
        if(!u_listen)
            return;
        
        /*从accept中读取socket之后，使用回调将数据保存在TcpServer*/
        int confd = u_listenfd.accept();
        if(confd <= 0)
            return;
        u_saveConnectCallBack(confd);
    }
}

