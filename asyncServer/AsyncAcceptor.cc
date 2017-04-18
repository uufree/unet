/*************************************************************************
	> File Name: AsyncAcceptor.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年04月17日 星期一 21时29分55秒
 ************************************************************************/

#include"AsyncAcceptor.h"

namespace unet
{
    namespace net
    {
        AsyncAcceptor::AsyncAcceptor(unet::net::InetAddress* addr_):
            serveraddr(addr_),
            listenfd(socket::socket()),
            listenchannel(new Channel(listenfd,false)),
            listening(false),
            channel_(nullptr)
        {
            int fd = socket::socket();
            socket::bind(fd,serveraddr);
            listenfd = fd;
            listenchannel->setFd(fd);

            listenchannel->setReadCallBack(std::bind(&AsyncAcceptor::handleRead,this));
        }

        AsyncAcceptor::~AsyncAcceptor()
        {
            ::close(listenfd);
        }

        void AsyncAcceptor::listen()
        {
            listening = true;
            socket::listen(listenfd);
            addinserverloop(listenchannel);
        }

        void AsyncAcceptor::handleRead()
        {
            int confd = socket::accept(listenfd);
            assert(confd >= 0);
            socket::setNonBlockAndCloseOnExec(confd);

            if(newconnectioncallback)
                channel_ = newconnectioncallback(confd);
            
            addinserverloop(channel_);
        }
    }
}

