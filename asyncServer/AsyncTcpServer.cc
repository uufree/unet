/*************************************************************************
	> File Name: AsyncTcpServer.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年04月17日 星期一 16时27分23秒
 ************************************************************************/

#include"AsyncTcpServer.h"

namespace unet
{
    namespace net
    {
        AsyncTcpServer::AsyncTcpServer(InetAddress* addr_,int size):
            serveraddr(addr_),
            loop(new AsyncEventLoop),
            epoller(new Epoller),
            acceptor(new AsyncAcceptor(addr_)),
            pool(new thread::TaskThreadPool(size))
        {
            acceptor->setNewConnectionCallBack(std::bind(&AsyncTcpServer::newConnectionCallBack,this,std::placeholders::_1));
            acceptor->setAddInServerLoopCallBack(std::bind(&AsyncTcpServer::addInServerLoop,this,std::placeholders::_1));
            
            loop->setGetActiveChannelsCallBack(std::bind(&AsyncTcpServer::getActiveChannels,this,std::placeholders::_1));
            loop->setHandleActiveChannelsCallBack(std::bind(&AsyncTcpServer::addChannelInPool,this,std::placeholders::_1));
        }
        
        Channel* AsyncTcpServer::newConnectionCallBack(int fd_)
        {
            Channel* channel = new Channel(fd_,true);
            TcpConnectionPtr ptr(channel->getTcpConnectionPtr());

            if(readcallback)
                ptr->setReadCallBack(readcallback);
            if(writecallback)
                ptr->setWriteCallBack(writecallback);
            if(drivedcallback)
                ptr->setDrivedCallBack(drivedcallback);
            
            ptr->setHandleDiedTcpConnection(std::bind(&AsyncTcpServer::handleDiedTcpConnection,this,std::placeholders::_1));
            tcpconnectionptrmap.insert({fd_,ptr});
            
            return channel;
        };

        void AsyncTcpServer::addInServerLoop(Channel* channel)
        {
            epoller->addInChannelMap(channel);
        }

        void AsyncTcpServer::start()
        {
            acceptor->listen();
            pool->start();
            loop->loop();
        }

        void AsyncTcpServer::handleDiedTcpConnection(int fd)
        {
            thread::MutexLockGuard guard(lock);
            tcpconnectionptrmap[fd]->handleChannel();
            tcpconnectionptrmap[fd].reset();
            tcpconnectionptrmap.erase(fd);
        }
    }
}



