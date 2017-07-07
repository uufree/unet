/*************************************************************************
	> File Name: Epoller.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月09日 星期四 00时01分25秒
 ************************************************************************/

#include"Epoller.h"
#include"error.h"
#include"EventList.h"

namespace unet
{
    namespace net
    {
        Epoller::Epoller(EventList& events) :
            eventList(events),
            epollfd(::epoll_create(65536))
        {
            if(epollfd < 0)
                unet::handleError(errno);
        }                     
  
        Epoller::~Epoller()
        {
            ::close(epollfd);
        }

        void Epoller::epoll(ChannelList& channelList,ChannelMap& channelMap)
        {
            int activeEvents = ::epoll_wait(epollfd,&*eventList.getEventList().begin(),static_cast<int>(eventList.size()),timeoutMs);

            if(activeEvents > 0)
                getActiveEvents(activeEvents,channelList,channelMap);
            else if(activeEvents == 0)
                std::cout << "nothing happened!" << std::endl; 
            else
                unet::handleError(errno);
        }

        void Epoller::getActiveEvents(int activeEvents,ChannelList& channeList,ChannelMap& channelMap)
        {
            int fd;
            auto events = eventList.getEventList();
            for(int i=0;i<activeEvents;++i)
            {
                if((events[i].events & EPOLLIN) || (events[i].events & EPOLLOUT))
                {
                    fd = events[i].data.fd;
                    Channel& channel = channelMap.findChannel(fd); 
                    channel.setRevent(events[i].events);

                    channeList.push_back(channel);
                }
            }
        }
        

    }
}

