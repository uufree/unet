/*************************************************************************
	> File Name: Epoller.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月09日 星期四 00时00分59秒
 ************************************************************************/

#ifndef _EPOLLER_H
#define _EPOLLER_H

#include"ChannelMap.h"
#include<sys/epoll.h>

namespace unet
{
    namespace net
    { 
        class Epoller final
        {
            static const int timeoutMs = 200;//默认poll阻塞200ms
            static const int KNoneEvent = 0;

            typedef std::vector<struct epoll_event> EventList;
            typedef std::vector<Channel&> ChannelList;

            public:    
                explicit Epoller();
                Epoller(const Epoller&) = delete;
                Epoller& operator=(const Epoller&) = delete;
                Epoller(Epoller&& lhs);
                Epoller& operator=(Epoller&& lhs);
                ~Epoller();

                void epoll(ChannelList& channelList,ChannelMap& channelMap);
            
            private:
                void getActiveEvents(int activeEvents,ChannelList& channelList,ChannelMap& channelMap);

            private:
                EventList eventList;//保存epollfd的数组
                int epollfd;//内核维护的epollfd表
        };    
    }
}


#endif

