/*************************************************************************
	> File Name: Acceptor.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月10日 星期五 08时45分54秒
 ************************************************************************/

#include"Acceptor.h"

namespace unet
{
    namespace net
    {
        Acceptor::Acceptor(InetAddress* addr_) :
            loop(new EventLoop),
            serveraddr(addr_),
            listenfd(socket::socket()),
            listenchannel(new Channel(listenfd,false)),
            epoller(new Epoller),
            listening(false)
        {
            int fd = socket::socket();
            socket::bind(fd,serveraddr);//设置监听套接字 
            listenfd = fd;
            listenchannel->setFd(fd);

            loop->setGetActiveChannelsCallBack(std::bind(&Acceptor::getActiveChannels,this,std::placeholders::_1));//向loop注册获得活动的channel的方法
            listenchannel->setReadCallBack(std::bind(&Acceptor::handleRead,this));//处理listenfd的可读事件
        }

        void Acceptor::listen()
        {//将listenfd设置为可读的，并且加入到epoller中
            listening = true;
            socket::listen(listenfd);
            epoller->addInChannelMap(listenchannel);
            epoller->getInfo();
        }

        void Acceptor::getActiveChannels(ChannelList* channels)
        {
            epoller->epoll(channels);//获得epoller中的活动事件
        }
        
        void Acceptor::handleRead()
        {//监听到的时间的处理方法
            int confd = socket::accept(listenfd);

            assert(confd >= 0);
            socket::setNonBlockAndCloseOnExec(confd);
            
            Channel* channel = newconnectioncallback(confd);//获得新的channel
            epoller->addInChannelMap(channel);//将channel添加到epoller中
            epoller->getInfo();
        }

    }
}

