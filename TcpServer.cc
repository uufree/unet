/*************************************************************************
	> File Name: TcpServer.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月11日 星期六 19时18分41秒
 ************************************************************************/

#include"TcpServer.h"

namespace unet
{
    namespace net
    {
        TcpServer::TcpServer(InetAddress* serveraddr_) : serveraddr(serveraddr_),acceptor(new Acceptor(serveraddr_))
        {
            acceptor->setNewConnectionCallBack(std::bind(&TcpServer::newConnectionCallBack,this,std::placeholders::_1));
        };
    
        Channel* TcpServer::newConnectionCallBack(int fd_)
        {
              Channel* channel = new Channel(fd_,true);
              TcpConnectionPtr ptr(channel->getTcpConnectionPtr());//得到Channel对象中的TcpConnectionPtr对象（重点在于让Channel中的TcpConnectionPtr对象将控制权交出来，只保留弱指针）
              ptr->setReadCallBack(readcallback);//设置处理消息的方式
              ptr->setWriteCallBack(writecallback);
              
              ptr->setHandleDiedTcpConnection(std::bind(&TcpServer::handleDiedTcpConnection,this,std::placeholders::_1));
              
              tcpconnectionptrmap.insert(make_pair(fd_,ptr));//将得到的结果插入map中
              return channel;//返回的Channel交由Epoller处理（放入Epoller的map中）
        }

        void TcpServer::start()
        {//启动Loop循环
            acceptor->listen();
            acceptor->startLoop();            
        }
 
        void TcpServer::handleDiedTcpConnection(int fd)
        {
            tcpconnectionptrmap[fd]->handleChannel();
            tcpconnectionptrmap[fd].reset();
            tcpconnectionptrmap.erase(fd);
        }
    }
}
        


