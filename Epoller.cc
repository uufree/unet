/*************************************************************************
	> File Name: Epoller.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月09日 星期四 00时01分25秒
 ************************************************************************/

#include"Epoller.h"
#include"error.h"

namespace unet
{
    namespace net
    {
        Epoller::Epoller() : 
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
            int activeEvents = ::epoll_wait(epollfd,&*eventList.begin(),static_cast<int>(eventList.size()),timeoutMs);

            if(activeEvents > 0)
                getActiveEvents(activeEvents,channelList,channelMap);
            else if(activeEvents == 0)
                std::cout << "nothing happened!" << std::endl; 
            else
                unet::handleError(errno);
        }

        void Epoller::getActiveEvents(int activeEvents,ChannelList& channeList,ChannelMap& channelMap)
        {//将事件源中的Channel的事件描述进行设置，然后将已有事件发生的Channel传递给ChannelList
            int fd;
            for(int i=0;i<activeEvents;++i)
            {
                if((eventList[i].events & EPOLLIN) || (eventList[i].events & EPOLLOUT))
                {
                    fd = eventList[i].data.fd;
                    Channel& channel = channelMap.findChannel(fd); 
                    channel.setRevent(eventList[i].events);

                    channeList.push_back(channel);
                }
            }
        }
        

    }
}

