/*************************************************************************
	> File Name: Epoller.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月09日 星期四 00时01分25秒
 ************************************************************************/

#include"Epoller.h"

namespace unet
{
    namespace net
    {
        Epoller::Epoller() :
            eventList(),
            epollfd(::epoll_create(65536))
        {
            if(epollfd < 0)
                unet::handleError(errno);
        }                     
        
        Epoller::Epoller(Epoller&& lhs) : 
            eventList(std::move(lhs.eventList)),
            epollfd(std::move(lhs.epollfd))
        {};
        
        Epoller& Epoller::operator=(Epoller&& lhs)
        {
            eventList = std::move(lhs.eventList);

            return *this;
        }
        
        Epoller::~Epoller()
        {
            ::close(epollfd);
        }

        void Epoller::epoll(ChannelList& channelList,ChannelMap& channelMap,TcpConnectionMap& tcpconnectionMap)
        {
            eventList.clear();
            int activeEvents = ::epoll_wait(epollfd,&*eventList.begin(),eventList.size(),timeoutMs);
             
            std::cout << "activeEvents: " << activeEvents << std::endl;

            if(activeEvents > 0)
                getActiveEvents(activeEvents,channelList,channelMap,tcpconnectionMap);
            else if(activeEvents == 0)
            {}
            else
            {
                std::cout << "epoll_wait error" << std::endl;
                unet::handleError(errno);
                std::cout << "epoll_wait error" << std::endl;
            }
        }

        void Epoller::getActiveEvents(int activeEvents,ChannelList& channeList,ChannelMap& channelMap,TcpConnectionMap& tcpConnectionMap)
        {
            int fd;

            for(int i=0;i<activeEvents;++i)
            {
                if((eventList[i].events & EPOLLIN) || (eventList[i].events & EPOLLOUT))
                {
                    fd = eventList[i].data.fd;
                    ChannelPtr& channel = channelMap.findChannel(fd); 
                    channel->setRevent(eventList[i].events);
                    
                    TcpConnectionPtr& con = tcpConnectionMap.find(fd);
                    con->read();

                    channeList.push_back(channel);
                }
            }
        }
        

    }
}

