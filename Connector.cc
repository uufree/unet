/*************************************************************************
	> File Name: Connector.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月21日 星期二 20时36分20秒
 ************************************************************************/

#include"Connector.h"
#include<sys/poll.h>

namespace unet
{
    namespace net
    {
        Connector::Connector(InetAddress* serveraddr_) : 
            confd(socket::socket()),
            serveraddr(serveraddr_),
            connected(false),
            epoller(new Epoller),
            loop(new EventLoop)
        {
            socket::setNonBlockAndCloseOnExec(confd.getFd());
        };

        void Connector::createChannel()
        {
            while(!createConnection())   
            {
                confd.setFd(socket::socket());    
                socket::setNonBlockAndCloseOnExec(confd.getFd());
            }
            
            connectchannel = new Channel(confd.getFd());
            connectioncallback((connectchannel->getTcpConnectionPtr()));
            epoller->addInChannelMap(connectchannel);
        }

        void Connector::start()
        {
            loop->loop();
        }

        int Connector::createConnection()
        {
            int n = socket::connect(confd.getFd(),serveraddr);
            if(n < 0)
            {
                if(errno != EINPROGRESS)
                {
                    ::close(confd.getFd());
                    return -1;
                }
            }
            else
                return confd.getFd();
            
            struct pollfd pfd;
            pfd.fd = confd.getFd();
            pfd.events = POLLOUT;

            int ret = ::poll(&pfd,1,50);
            if(ret == 1)
            {
                if((pfd.revents & POLLOUT) && pfd.revents & !POLLIN)
                {
                    socklen_t sock;
                    if(getsockopt(confd.getFd(),SOL_SOCKET,SO_ERROR,NULL,&sock) < 0)
                    {
                        ::close(confd.getFd());
                        return -1;
                    }
                    else
                        return confd.getFd();
                }

                else if(pfd.revents & POLLIN)
                {
                    ::close(confd.getFd());
                    return -1;
                }
                else
                {
                    ::close(confd.getFd());
                    return -1;
                }
            }
            else
                return -1;
        }
    } 

}            








































